// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/MobSpawnManager.h"
#include "Character/MobGASCharacter.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameData/MobSpawnInfo.h" 
#include "GameData/BossSpawnInfo.h" 

AMobSpawnManager::AMobSpawnManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMobSpawnManager::BeginPlay()
{
    Super::BeginPlay();

    RequestRandomSpawnWave();
    FTimerHandle SpawnTimer;
    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AMobSpawnManager::RequestRandomSpawnWave, 10.0f, true);
    
    if (FindSpawnLocation(BossSpawnLocation))
    {
        //해당 위치에 상징 구조물을 먼저 스폰 (설정된 경우)
        if (BossSymbolicStructureClass)
        {
            GetWorld()->SpawnActor<AActor>(BossSymbolicStructureClass, BossSpawnLocation, FRotator::ZeroRotator);
        }
    }

    //5분(BossSpawnTime) 후에 보스를 스폰하도록 타이머 설정
    FTimerHandle BossSpawnTimer;
    GetWorld()->GetTimerManager().SetTimer(BossSpawnTimer, this, &AMobSpawnManager::SpawnBoss, BossSpawnTime, false);
}

void AMobSpawnManager::SpawnBoss()
{
    if (!BossSpawnTable || BossDataRowName.IsNone()) return;

    FString ContextString;
    FBossSpawnInfo* BossInfo = BossSpawnTable->FindRow<FBossSpawnInfo>(BossDataRowName, ContextString);

    if (BossInfo)   // && GameMode
    {
		const float DifficultyMultiplier = 1.0f; //GameMode->GetCurrentDifficultyMultiplier();
        SpawnSingleBoss(BossInfo, BossSpawnLocation, DifficultyMultiplier);
    }
}

void AMobSpawnManager::RequestRandomSpawnWave()
{
    if (!MonsterSpawnTable) return;

    const TArray<FName> RowNames = MonsterSpawnTable->GetRowNames();

    if (RowNames.Num() > 0)
    {
        const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
        RequestSpawnWave(RowNames[RandomIndex]);
    }
}

void AMobSpawnManager::RequestSpawnWave(const FName& WaveDataRowName)
{
    if (!MonsterSpawnTable) return;

    FString ContextString;
    FMobSpawnInfo* SpawnInfo = MonsterSpawnTable->FindRow<FMobSpawnInfo>(WaveDataRowName, ContextString);

    if (SpawnInfo)  // && GameMode
    {
        // 웨이브가 생성될 중심 위치
        FVector WaveCenterLocation;
        if (FindSpawnLocation(WaveCenterLocation))
        {
            // 난이도 배수를 가져옴
            const float DifficultyMultiplier = 1.0f;//GameMode->GetCurrentDifficultyMultiplier();

            //SpawnCount만큼 몬스터를 중심점 주변에 스폰
            for (int32 i = 0; i < SpawnInfo->SpawnCount; ++i)
            {
                UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
                if (NavSys)
                {
                    FNavLocation SpawnLocation;
                    // 중심점 주변 반경 내에서 네비게이션 가능한 위치를 찾음
                    if (NavSys->GetRandomPointInNavigableRadius(WaveCenterLocation, 500.0f, SpawnLocation))     //500.0f 반경
                    {
                        // 실제 몬스터 스폰
                        SpawnSingleMonster(SpawnInfo, SpawnLocation.Location, DifficultyMultiplier);
                    }
                }
            }
        }
    }
}

bool AMobSpawnManager::FindSpawnLocation(FVector& OutLocation)
{
    ANavMeshBoundsVolume* NavVolume = Cast<ANavMeshBoundsVolume>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass())
    );

    if (!NavVolume) return false;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        const FBox BoundingBox = NavVolume->GetBounds().GetBox();
        for (int32 i = 0; i < 10; ++i)
        {
            const FVector RandomPointInBox = UKismetMathLibrary::RandomPointInBoundingBox(BoundingBox.Min, BoundingBox.Max);
            FNavLocation NavLocation;
            if (NavSys->GetRandomPointInNavigableRadius(RandomPointInBox, 500.f, NavLocation))
            {
                OutLocation = NavLocation.Location;
                return true;
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


	FVector SpawnLocation = Location;
	SpawnLocation.Z += 200.0f; // 약간 위로 스폰 위치 조정 (지면에서 떨어뜨리기)
    AMobGASCharacter* NewMonster = GetWorld()->SpawnActor<AMobGASCharacter>(SpawnInfo->MonsterClass, Location, FRotator::ZeroRotator);

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
void AMobSpawnManager::SpawnSingleBoss(const FBossSpawnInfo* BossInfo, const FVector& Location, float DifficultyMultiplier)
{
    if (!BossInfo) return;

    if (BossInfo->SpawnVFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BossInfo->SpawnVFX, Location);
    if (BossInfo->SpawnSFX) UGameplayStatics::PlaySoundAtLocation(GetWorld(), BossInfo->SpawnSFX, Location);

    // 보스 스폰 시에는 FSpawnData가 아닌 FBossSpawnInfo를 사용하므로, InitializeFromData를 직접 호출하기 어려움.
    // 보스 캐릭터에 별도의 초기화 함수를 만들거나, 스탯을 직접 설정해야 함. 여기서는 스폰만 진행.
	FVector SpawnLocation = Location;
	SpawnLocation.Z += 200.0f; // 약간 위로 스폰 위치 조정 (지면에서 떨어뜨리기)
    ANonPlayerGASCharacter* NewBoss = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(BossInfo->BossClass, SpawnLocation, FRotator::ZeroRotator);
    if (NewBoss)
    {
        // TODO: 보스 캐릭터를 위한 별도의 데이터 초기화 함수 호출 (예: NewBoss->InitializeFromBossData(BossInfo);)

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
