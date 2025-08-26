// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/NonPoolingMobSpawnManager.h"
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
ANonPoolingMobSpawnManager::ANonPoolingMobSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANonPoolingMobSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	//ChooseRandomMob();
	FTimerHandle SpawnTimer;
	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &ANonPoolingMobSpawnManager::ChooseRandomMob, SpawnDelay, true);

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
	GetWorld()->GetTimerManager().SetTimer(BossSpawnTimer, this, &ANonPoolingMobSpawnManager::SpawnBoss, BossSpawnTime, false);
}

void ANonPoolingMobSpawnManager::SpawnBoss()
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
void ANonPoolingMobSpawnManager::ChooseRandomMob()
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
void ANonPoolingMobSpawnManager::RequestSpawnWave(const FName& WaveDataRowName)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(NonPooling_RequestSpawnWave);

	if (!MonsterSpawnTable) return;

	FString ContextString;
	FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(WaveDataRowName, ContextString);

	if (SpawnInfo)
	{
		// 웨이브가 생성될 중심 위치
		FVector WaveCenterLocation;
		if (FindSpawnLocation(SpawnInfo->MonsterClass, WaveCenterLocation))
		{
			const float DifficultyMultiplier = 1.0f;

			for (int32 i = 0; i < SpawnInfo->SpawnCount; ++i)
			{
				UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
				if (NavSys)
				{
					FNavLocation SpawnLocation;
					if (NavSys->GetRandomPointInNavigableRadius(WaveCenterLocation, 500.0f, SpawnLocation))
					{
						SpawnSingleMonster(SpawnInfo, SpawnLocation.Location, DifficultyMultiplier);
					}
				}
			}
		}
	}
}

bool ANonPoolingMobSpawnManager::FindSpawnLocation(TSubclassOf<AActor> ActorClass, FVector& OutLocation)
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

void ANonPoolingMobSpawnManager::SpawnSingleMonster(const FMobSpawnInfo* SpawnInfo, const FVector& Location, float DifficultyMultiplier)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(NonPooling_SpawnSingleMonster);

	if (!SpawnInfo) return;

	if (SpawnInfo->SpawnVFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnInfo->SpawnVFX, Location);
	if (SpawnInfo->SpawnSFX) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnInfo->SpawnSFX, Location);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMobGASCharacter* NewMonster = GetWorld()->SpawnActor<AMobGASCharacter>(SpawnInfo->MonsterClass, Location, FRotator::ZeroRotator, SpawnParams);

	if (NewMonster)
	{
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
void ANonPoolingMobSpawnManager::SpawnSingleBoss(const FBossSpawnInfo* BossInfo, const FVector& Location, float DifficultyMultiplier)
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


