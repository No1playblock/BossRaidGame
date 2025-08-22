// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/MobSpawnInfo.h" // FMonsterSpawnInfo ����ü
#include "GameData/BossSpawnInfo.h" // FBossSpawnInfo ����ü
#include "MobSpawnManager.generated.h"

class UGameplayEffect;
struct FMobSpawnInfo;
struct FBossSpawnInfo;
class ANonPlayerGASCharacter;

/*TImeSlicing ����ü*/
//USTRUCT()
//struct FSpawnQueueItem
//{
//	GENERATED_BODY()
//
//	UPROPERTY()
//	FMobSpawnInfo SpawnInfo;
//
//	UPROPERTY()
//	FVector Location;
//
//	UPROPERTY()
//	float DifficultyMultiplier = 1.0f;
//};
USTRUCT()
struct FMonsterPool
{
	GENERATED_BODY()

	// Ǯ�� ������ ���͵� (GC�� �������� UPROPERTY + TObjectPtr)
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

	/*UFUNCTION()
	void ProcessSpawnQueue();*/

private:

	void InitializePool();
	ANonPlayerGASCharacter* GetPooledMonster(TSubclassOf<ACharacter> MonsterClass);

	void ChooseRandomMob();
	void RequestSpawnWave(const FName& WaveDataRowName);
	//void ProcessSpawnQueue();

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

	// ���� ������ �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Config")
	float SpawnDelay = 0.5f;

	// ���� ���� ��ġ ����
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Boss")
	TSubclassOf<AActor> BossSymbolicStructureClass;


	// ���� ���� �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Spawn | Boss")
	float BossSpawnTime = 300.0f;

	FVector BossSpawnLocation;

	// ��Ȱ��ȭ�� ���͵��� �������� �����ϴ� ��
	UPROPERTY()
	TMap<TSubclassOf<ACharacter>, FMonsterPool> InactiveMonsterPool;

	
	/*TimeSlicing�� ����ϱ� ���� ����*/
	//TQueue<TPair<const FMobSpawnInfo*, FVector>> SpawnQueue;

	//// ť�� ó���� Ÿ�̸� �ڵ�
	//FTimerHandle SpawnQueueTimerHandle;

};
