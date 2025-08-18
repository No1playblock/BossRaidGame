// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/MobSpawnManager.h"
#include "Character/MobGASCharacter.h"
#include "Character/BossGASCharacter.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameData/MobSpawnInfo.h" 
#include "GameData/BossSpawnInfo.h" 
#include "Components/CapsuleComponent.h"
AMobSpawnManager::AMobSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMobSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	InitializePool();

	ChooseRandomMob();
	FTimerHandle RequestTimer;
	GetWorld()->GetTimerManager().SetTimer(RequestTimer, this, &AMobSpawnManager::ChooseRandomMob, 10.0f, true);
	GetWorld()->GetTimerManager().SetTimer(SpawnQueueTimerHandle, this, &AMobSpawnManager::ProcessSpawnQueue, 0.1f, true); // 큐 처리 간격을 약간 늘림

	UE_LOG(LogTemp, Warning, TEXT("MobSpawnManager BeginPlay: StartSpawnManager"));

	if (FindSpawnLocation(BossSymbolicStructureClass, BossSpawnLocation))
	{
		if (BossSymbolicStructureClass)
		{
			GetWorld()->SpawnActor<AActor>(BossSymbolicStructureClass, BossSpawnLocation, FRotator::ZeroRotator);
		}
	}

	//5분(BossSpawnTime) 후에 보스를 스폰하도록 타이머 설정
	FTimerHandle BossSpawnTimer;
	//GetWorld()->GetTimerManager().SetTimer(BossSpawnTimer, this, &AMobSpawnManager::SpawnBoss, BossSpawnTime, false);
}
void AMobSpawnManager::InitializePool()
{
	if (!MonsterSpawnTable) return;

	const TArray<FName> RowNames = MonsterSpawnTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(RowName, TEXT(""));
		if (SpawnInfo && SpawnInfo->MonsterClass)
		{
			// 해당 몬스터 클래스의 풀을 가져오거나 새로 생성
			FMonsterPool& PoolStruct = InactiveMonsterPool.FindOrAdd(SpawnInfo->MonsterClass);

			for (int32 i = 0; i < InitialPoolSize; ++i)
			{
				// 몬스터를 스폰하고 즉시 비활성화하여 풀에 추가
				ANonPlayerGASCharacter* NewMonster = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(
					SpawnInfo->MonsterClass,
					FVector(-10000.f, -10000.f, -10000.f), // 보이지 않는 먼 위치
					FRotator::ZeroRotator
				);

				if (NewMonster)
				{
					NewMonster->DeactivateCharacter(); // 비활성화
					PoolStruct.Inactive.Add(NewMonster); // 구조체의 배열에 추가
				}
			}
		}
	}
}

ANonPlayerGASCharacter* AMobSpawnManager::GetPooledMonster(TSubclassOf<ACharacter> MonsterClass)
{
	// .Inactive 를 통해 배열에 접근
	TArray<TObjectPtr<ANonPlayerGASCharacter>>& Pool = InactiveMonsterPool.FindOrAdd(MonsterClass).Inactive;

	// 유효한 것 꺼내기 (뒤에서 팝)
	while (Pool.Num() > 0)
	{
		TObjectPtr<ANonPlayerGASCharacter> Obj = Pool.Pop(false);
		if (IsValid(Obj.Get()))
		{
			return Obj.Get();
		}
	}
	return nullptr;
}

void AMobSpawnManager::ReturnMonsterToPool(ANonPlayerGASCharacter* Monster)
{
	if (!IsValid(Monster)) return;

	Monster->DeactivateCharacter();

	// 클래스 키로 풀에 넣기
	TSubclassOf<ACharacter> Cls = Monster->GetClass();
	InactiveMonsterPool.FindOrAdd(Cls).Inactive.Add(Monster);
}
void AMobSpawnManager::SpawnBoss()
{
	if (!BossSpawnTable || BossDataRowName.IsNone()) return;

	FString ContextString;
	FBossSpawnInfo* BossInfo = BossSpawnTable->FindRow<FBossSpawnInfo>(BossDataRowName, ContextString);

	if (BossInfo)
	{
		const float DifficultyMultiplier = 1.0f;
		SpawnSingleBoss(BossInfo, BossSpawnLocation, DifficultyMultiplier);
	}
}

//랜덤한 몹을 생성
void AMobSpawnManager::ChooseRandomMob()
{
	if (!MonsterSpawnTable) return;

	const TArray<FName> RowNames = MonsterSpawnTable->GetRowNames();

	if (RowNames.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
		RequestSpawnWave(RowNames[RandomIndex]);
	}
}

//실제 몹 생성
void AMobSpawnManager::RequestSpawnWave(const FName& WaveDataRowName)
{
	if (!MonsterSpawnTable) return;

	FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(WaveDataRowName, TEXT(""));
	if (!SpawnInfo) return;

	// 멀티스레딩 대신 동기 방식으로 위치 탐색 (풀링이 더 중요한 최적화)
	FVector WaveCenterLocation;
	if (FindSpawnLocation(SpawnInfo->MonsterClass, WaveCenterLocation))
	{
		for (int32 i = 0; i < SpawnInfo->SpawnCount; ++i)
		{
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			if (NavSys)
			{
				FNavLocation SpawnLocation;
				if (NavSys->GetRandomPointInNavigableRadius(WaveCenterLocation, 500.0f, SpawnLocation))
				{
					SpawnLocation.Location.Z = WaveCenterLocation.Z;
					// 스폰 큐에 데이터 추가
					SpawnQueue.Enqueue(TPair<const FMobSpawnInfo*, FVector>(SpawnInfo, SpawnLocation.Location));
				}
			}
		}
	}
}
void AMobSpawnManager::ProcessSpawnQueue()
{
	if (SpawnQueue.IsEmpty()) return;

	TPair<const FMobSpawnInfo*, FVector> ItemToSpawn;
	if (SpawnQueue.Dequeue(ItemToSpawn))
	{
		SpawnSingleMonster(ItemToSpawn.Key, ItemToSpawn.Value, 1.0f);
	}
}
bool AMobSpawnManager::FindSpawnLocation(TSubclassOf<AActor> ActorClass, FVector& OutLocation)
{
	ANavMeshBoundsVolume* NavVolume = Cast<ANavMeshBoundsVolume>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass())
	);
	if (!NavVolume || !ActorClass) return false;

	float CheckHeightOffset = 0.f;
	FCollisionShape CollisionShape;

	AActor* DefaultActor = ActorClass->GetDefaultObject<AActor>();

	if (ACharacter* DefaultCharacter = Cast<ACharacter>(DefaultActor))
	{
		const float CapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		CheckHeightOffset = CapsuleHalfHeight;
		CollisionShape = FCollisionShape::MakeCapsule(DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(), CapsuleHalfHeight);
	}
	else
	{
		CheckHeightOffset = 20.f;
		CollisionShape = FCollisionShape::MakeBox(FVector(50.f, 50.f, 20.f));
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		const FBox BoundingBox = NavVolume->GetBounds().GetBox();
		for (int32 i = 0; i < 10; ++i)
		{
			FNavLocation NavLocation;
			if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), BoundingBox.GetExtent().Length(), NavLocation))
			{
				FVector CheckLocation = NavLocation.Location;
				CheckLocation.Z += CheckHeightOffset;

				FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpawnCheck), false, this);
				bool bHasBlockingOverlap = GetWorld()->OverlapBlockingTestByChannel(
					CheckLocation,
					FQuat::Identity,
					ECollisionChannel::ECC_Pawn,
					CollisionShape,
					QueryParams
				);

				//캐릭터들의 피벗이 HalfHeight에 위치하므로,
				if (!bHasBlockingOverlap)
				{
					OutLocation = CheckLocation;
					return true;
				}
			}
		}
	}
	return false;
}

void AMobSpawnManager::SpawnSingleMonster(const FMobSpawnInfo* SpawnInfo, const FVector& Location, float DifficultyMultiplier)
{
	if (!SpawnInfo) return;

	if (SpawnInfo->SpawnVFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnInfo->SpawnVFX, Location);
	if (SpawnInfo->SpawnSFX) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnInfo->SpawnSFX, Location);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AMobGASCharacter* NewMonster = Cast<AMobGASCharacter>(GetPooledMonster(SpawnInfo->MonsterClass));
	if (NewMonster)
	{
		// 가져온 몬스터를 활성화하고 위치 설정
		NewMonster->SetActorLocation(Location);
		NewMonster->ActivateCharacter();
		NewMonster->SetOwningSpawnManager(this); // 풀로 돌아올 수 있도록 매니저 정보 전달
		NewMonster->InitializeFromData(SpawnInfo);

		UE_LOG(LogTemp, Warning, TEXT("Spawned Monster: %s at Location: %s"), *NewMonster->GetName(), *Location.ToString());
		UAbilitySystemComponent* AbilitySystem = NewMonster->GetAbilitySystemComponent();
		if (AbilitySystem && StatScalingEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(StatScalingEffectClass, DifficultyMultiplier, ContextHandle);
			if (SpecHandle.IsValid())
			{
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
void AMobSpawnManager::SpawnSingleBoss(const FBossSpawnInfo* BossInfo, const FVector& Location, float DifficultyMultiplier)
{
	if (!BossInfo) return;

	if (BossInfo->SpawnVFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BossInfo->SpawnVFX, Location);
	if (BossInfo->SpawnSFX) UGameplayStatics::PlaySoundAtLocation(GetWorld(), BossInfo->SpawnSFX, Location);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


	BossSpawnLocation.Z += BossInfo->BossClass->GetDefaultObject<ABossGASCharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	ABossGASCharacter* NewBoss = GetWorld()->SpawnActor<ABossGASCharacter>(BossInfo->BossClass, Location, FRotator::ZeroRotator, SpawnParams);

	if (NewBoss)
	{
		NewBoss->InitializeFromData(BossInfo);
		UAbilitySystemComponent* AbilitySystem = NewBoss->GetAbilitySystemComponent();
		if (AbilitySystem && StatScalingEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(StatScalingEffectClass, DifficultyMultiplier, ContextHandle);
			if (SpecHandle.IsValid())
			{
				AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
