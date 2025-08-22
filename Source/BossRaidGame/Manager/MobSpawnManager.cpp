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
#include "Stats/Stats.h"

DECLARE_STATS_GROUP(TEXT("MobSpawning"), STATGROUP_MobSpawning, STATCAT_Advanced);
// ����(Synchronous) ����� FindSpawnLocation �Լ��� ������ ���� ID ����
DECLARE_CYCLE_STAT(TEXT("Sync_FindSpawnLocation"), STAT_Sync_FindSpawnLocation, STATGROUP_MobSpawning);

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
	GetWorld()->GetTimerManager().SetTimer(RequestTimer, this, &AMobSpawnManager::ChooseRandomMob, SpawnDelay, true);
	//GetWorld()->GetTimerManager().SetTimer(SpawnQueueTimerHandle, this, &AMobSpawnManager::ProcessSpawnQueue, 0.1f, true); // ť ó�� ������ �ణ �ø�

	UE_LOG(LogTemp, Warning, TEXT("MobSpawnManager BeginPlay: StartSpawnManager"));

	if (FindSpawnLocation(BossSymbolicStructureClass, BossSpawnLocation))
	{
		if (BossSymbolicStructureClass)
		{
			GetWorld()->SpawnActor<AActor>(BossSymbolicStructureClass, BossSpawnLocation, FRotator::ZeroRotator);
		}
	}

	//5��(BossSpawnTime) �Ŀ� ������ �����ϵ��� Ÿ�̸� ����
	FTimerHandle BossSpawnTimer;
	//GetWorld()->GetTimerManager().SetTimer(BossSpawnTimer, this, &AMobSpawnManager::SpawnBoss, BossSpawnTime, false);

}
/*������ ��ȯ*/
//void AMobSpawnManager::InitializePool()
//{
//	if (!MonsterSpawnTable) return;
//
//	const TArray<FName> RowNames = MonsterSpawnTable->GetRowNames();
//	for (const FName& RowName : RowNames)
//	{
//		FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(RowName, TEXT(""));
//		if (SpawnInfo && SpawnInfo->MonsterClass)
//		{
//			// �ش� ���� Ŭ������ Ǯ�� �������ų� ���� ����
//			FMonsterPool& PoolStruct = InactiveMonsterPool.FindOrAdd(SpawnInfo->MonsterClass);
//
//			for (int32 i = 0; i < InitialPoolSize; ++i)
//			{
//
//				FActorSpawnParameters SpawnParams;
//				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//
//				// ���͸� �����ϰ� ��� ��Ȱ��ȭ�Ͽ� Ǯ�� �߰�
//				ANonPlayerGASCharacter* NewMonster = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(
//					SpawnInfo->MonsterClass,
//					FVector(-10000.f, -10000.f, -10000.f), // ������ �ʴ� �� ��ġ
//					FRotator::ZeroRotator,
//					SpawnParams
//				);
//
//				if (NewMonster)
//				{
//					NewMonster->DeactivateCharacter(); // ��Ȱ��ȭ
//					PoolStruct.Inactive.Add(NewMonster); // ����ü�� �迭�� �߰�
//				}
//			}
//		}
//	}
//}
void AMobSpawnManager::InitializePool()
{
	if (!MonsterSpawnTable) return;

	const TArray<FName> RowNames = MonsterSpawnTable->GetRowNames();
	// RowNames �迭�� ��ȸ�ϸ鼭 �ε����� ���� ���
	for (int32 SpeciesIndex = 0; SpeciesIndex < RowNames.Num(); ++SpeciesIndex)
	{
		const FName& RowName = RowNames[SpeciesIndex];
		FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(RowName, TEXT(""));

		if (SpawnInfo && SpawnInfo->MonsterClass)
		{
			FMonsterPool& PoolStruct = InactiveMonsterPool.FindOrAdd(SpawnInfo->MonsterClass);


			const FVector SpeciesOffset = FVector(0.f, SpeciesIndex * 5000.f, 0.f);
			const FVector PoolStartPosition = FVector(-20000.f, -20000.f, -10000.f) + SpeciesOffset;

			const float Spacing = 200.0f;
			const int32 GridWidth = 10;

			for (int32 i = 0; i < InitialPoolSize; ++i)
			{
				const int32 Row = i / GridWidth;
				const int32 Col = i % GridWidth;
				const FVector SpawnLocation = PoolStartPosition + FVector(Col * Spacing, Row * Spacing, 0.f);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				ANonPlayerGASCharacter* NewMonster = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(
					SpawnInfo->MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams
				);

				if (NewMonster)
				{
					NewMonster->SetOwningSpawnManager(this);
					NewMonster->DeactivateCharacter();
					PoolStruct.Inactive.Add(NewMonster);
				}
			}
		}
	}
}
ANonPlayerGASCharacter* AMobSpawnManager::GetPooledMonster(TSubclassOf<ACharacter> MonsterClass)
{
	// .Inactive �� ���� �迭�� ����
	TArray<TObjectPtr<ANonPlayerGASCharacter>>& Pool = InactiveMonsterPool.FindOrAdd(MonsterClass).Inactive;

	// ��ȿ�� �� ������ (�ڿ��� ��)
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

	// Ŭ���� Ű�� Ǯ�� �ֱ�
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

//������ ���� ����
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

//���� �� ����
void AMobSpawnManager::RequestSpawnWave(const FName& WaveDataRowName)
{
	if (!MonsterSpawnTable) return;

	FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(WaveDataRowName, TEXT(""));
	if (!SpawnInfo) return;

	// ��Ƽ������ ��� ���� ������� ��ġ Ž�� (Ǯ���� �� �߿��� ����ȭ)
	FVector WaveCenterLocation;
	if (FindSpawnLocation(SpawnInfo->MonsterClass, WaveCenterLocation))
	{
		for (int32 i = 0; i < SpawnInfo->SpawnCount; ++i)
		{
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			if (NavSys)
			{
				FNavLocation SpawnLocation;
				if (NavSys->GetRandomPointInNavigableRadius(WaveCenterLocation, 700.0f, SpawnLocation))
				{
					SpawnLocation.Location.Z = WaveCenterLocation.Z;
					// ���� ť�� ������ �߰�
					SpawnSingleMonster(SpawnInfo, SpawnLocation.Location, 1.0f);
					//SpawnQueue.Enqueue(TPair<const FMobSpawnInfo*, FVector>(SpawnInfo, SpawnLocation.Location));

				}
			}
		}
	}
}
/*TimeSlicing �Լ�*/
//void AMobSpawnManager::ProcessSpawnQueue()
//{
//	if (SpawnQueue.IsEmpty()) return;
//
//	TPair<const FMobSpawnInfo*, FVector> ItemToSpawn;
//	if (SpawnQueue.Dequeue(ItemToSpawn))
//	{
//		SpawnSingleMonster(ItemToSpawn.Key, ItemToSpawn.Value, 1.0f);
//	}
//}
bool AMobSpawnManager::FindSpawnLocation(TSubclassOf<AActor> ActorClass, FVector& OutLocation)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FindSpawnLocation_Sync);
	SCOPE_CYCLE_COUNTER(STAT_Sync_FindSpawnLocation);

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

				//ĳ���͵��� �ǹ��� HalfHeight�� ��ġ�ϹǷ�,
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
		// ������ ���͸� Ȱ��ȭ�ϰ� ��ġ ����
		NewMonster->SetActorLocation(Location);		
		NewMonster->SetOwningSpawnManager(this); // Ǯ�� ���ƿ� �� �ֵ��� �Ŵ��� ���� ����
		NewMonster->InitializeFromData(SpawnInfo);
		NewMonster->ActivateCharacter();

		//UE_LOG(LogTemp, Warning, TEXT("Spawned Monster: %s at Location: %s"), *NewMonster->GetName(), *Location.ToString());
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
