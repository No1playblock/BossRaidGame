// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/MobSpawnManager.h"
//#include "YourGameMode.h" // ������Ʈ�� GameMode Ŭ���� ���
#include "Character/NonPlayerGASCharacter.h" // ������Ʈ�� Monster ĳ���� Ŭ���� ���
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
    // �׽�Ʈ��: ���� ���� 5�� �� "InitialWave" ��� �̸��� ���̺� ����
    FTimerHandle SpawnTimer;
    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AMobSpawnManager::RequestRandomSpawnWave, 10.0f, true);
    
    if (FindSpawnLocation(BossSpawnLocation))
    {
        // 3. �ش� ��ġ�� ��¡ �������� ���� ���� (������ ���)
        if (BossSymbolicStructureClass)
        {
            GetWorld()->SpawnActor<AActor>(BossSymbolicStructureClass, BossSpawnLocation, FRotator::ZeroRotator);
        }
    }

    // 4. 5��(BossSpawnTime) �Ŀ� ������ �����ϵ��� Ÿ�̸� ����
    FTimerHandle BossSpawnTimer;
    GetWorld()->GetTimerManager().SetTimer(BossSpawnTimer, this, &AMobSpawnManager::SpawnBoss, BossSpawnTime, false);
}

void AMobSpawnManager::SpawnBoss()
{
    if (!BossSpawnTable || BossDataRowName.IsNone()) return;

    FString ContextString;
    FBossSpawnInfo* BossInfo = BossSpawnTable->FindRow<FBossSpawnInfo>(BossDataRowName, ContextString);
    //ABRGGameMode* GameMode = GetWorld()->GetAuthGameMode<ABRGGameMode>();

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
        // 1. ���̺갡 ������ �߽� ��ġ�� ���� ã��
        FVector WaveCenterLocation;
        if (FindSpawnLocation(WaveCenterLocation))
        {
            // 2. ���� ���̵� ����� ������ (�� ���� ȣ��)
            const float DifficultyMultiplier = 1.0f;//GameMode->GetCurrentDifficultyMultiplier();

            // 3. SpawnCount��ŭ ���͸� �߽��� �ֺ��� ����
            for (int32 i = 0; i < SpawnInfo->SpawnCount; ++i)
            {
                UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
                if (NavSys)
                {
                    FNavLocation SpawnLocation;
                    // �߽��� �ֺ� �ݰ�(SpawnRadius) ������ �׺���̼� ������ ��ġ�� ã��
                    if (NavSys->GetRandomPointInNavigableRadius(WaveCenterLocation, 500.0f, SpawnLocation))     //500.0f �ݰ�
                    {
                        // ���� ���� ����
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
	SpawnLocation.Z += 200.0f; // �ణ ���� ���� ��ġ ���� (���鿡�� ����߸���)
    ANonPlayerGASCharacter* NewMonster = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(SpawnInfo->MonsterClass, Location, FRotator::ZeroRotator);

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

    // ���� ���� �ÿ��� FSpawnData�� �ƴ� FBossSpawnInfo�� ����ϹǷ�, InitializeFromData�� ���� ȣ���ϱ� �����.
    // ���� ĳ���Ϳ� ������ �ʱ�ȭ �Լ��� ����ų�, ������ ���� �����ؾ� ��. ���⼭�� ������ ����.
	FVector SpawnLocation = Location;
	SpawnLocation.Z += 200.0f; // �ణ ���� ���� ��ġ ���� (���鿡�� ����߸���)
    ANonPlayerGASCharacter* NewBoss = GetWorld()->SpawnActor<ANonPlayerGASCharacter>(BossInfo->BossClass, SpawnLocation, FRotator::ZeroRotator);
    if (NewBoss)
    {
        // TODO: ���� ĳ���͸� ���� ������ ������ �ʱ�ȭ �Լ� ȣ�� (��: NewBoss->InitializeFromBossData(BossInfo);)

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
