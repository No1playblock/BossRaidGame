// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/MobSpawnInfo.h" // FMonsterSpawnInfo 구조체
#include "GameData/BossSpawnInfo.h" // FBossSpawnInfo 구조체
#include "MobSpawnManager.generated.h"

class UGameplayEffect;
struct FMobSpawnInfo;
struct FBossSpawnInfo;
class ANonPlayerGASCharacter;

USTRUCT()
struct FSpawnQueueItem
{
	GENERATED_BODY()

	UPROPERTY()
	FMobSpawnInfo SpawnInfo;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	float DifficultyMultiplier = 1.0f;
};
USTRUCT()
struct FMonsterPool
{
	GENERATED_BODY()

	// 풀에 보관할 몬스터들 (GC에 잡히도록 UPROPERTY + TObjectPtr)
	UPROPERTY()
	TArray<TObjectPtr<ANonPlayerGASCharacter>> Inactive;
};
UCLASS()
class BOSSRAIDGAME_API AMobSpawnManager : public AActor
{
	GENERATED_BODY()

public:
	AMobSpawnManager();

	void ReturnMonsterToPool(ANonPlayerGASCharacter* Monster);


protected:
	virtual void BeginPlay() override;

private:

	void InitializePool();
	ANonPlayerGASCharacter* GetPooledMonster(TSubclassOf<ACharacter> MonsterClass);

	void ChooseRandomMob();
	void RequestSpawnWave(const FName& WaveDataRowName);
	void ProcessSpawnQueue();

	UFUNCTION()
	void SpawnBoss();

	bool FindSpawnLocation(TSubclassOf<AActor> ActorClass, FVector& OutLocation);

	void SpawnSingleMonster(const FMobSpawnInfo* SpawnInfo, const FVector& Location, float DifficultyMultiplier);

	void SpawnSingleBoss(const FBossSpawnInfo* BossInfo, const FVector& Location, float DifficultyMultiplier);


private:

	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Pool")
	int32 InitialPoolSize = 20; 

	UPROPERTY(EditDefaultsOnly, Category = "Spawn | General Mob")
	UDataTable* MonsterSpawnTable;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Boss")
	UDataTable* BossSpawnTable;

	UPROPERTY(EditInstanceOnly, Category = "Spawn | Boss")
	FName BossDataRowName = TEXT("1");

	UPROPERTY(EditDefaultsOnly, Category = "Spawn | GAS")
	TSubclassOf<UGameplayEffect> StatScalingEffectClass;

	// 스폰 딜레이 시간
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Config")
	float SpawnDelay = 0.5f;

	// 보스 스폰 위치 액터
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Boss")
	TSubclassOf<AActor> BossSymbolicStructureClass;


	// 보스 스폰 시간
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Boss")
	float BossSpawnTime = 300.0f;

	FVector BossSpawnLocation;

	// 비활성화된 몬스터들을 종류별로 보관하는 맵
	UPROPERTY()
	TMap<TSubclassOf<ACharacter>, FMonsterPool> InactiveMonsterPool;

	// 스폰 요청을 저장할 큐 (이제 포인터 대신 실제 데이터를 복사)
	TQueue<TPair<const FMobSpawnInfo*, FVector>> SpawnQueue;

	// 큐를 처리할 타이머 핸들
	FTimerHandle SpawnQueueTimerHandle;
};
