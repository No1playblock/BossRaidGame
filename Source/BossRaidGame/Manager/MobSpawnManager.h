// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/MobSpawnInfo.h" // FMonsterSpawnInfo ����ü
#include "GameData/BossSpawnInfo.h" // FBossSpawnInfo ����ü
#include "MobSpawnManager.generated.h"

class UGameplayEffect;


UCLASS()
class BOSSRAIDGAME_API AMobSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
    AMobSpawnManager();

    // ������ ���̺��� �� ��(Row)�� ������� ���� ���̺� ������ ��û�ϴ� �Լ�
    void RequestSpawnWave(const FName& WaveDataRowName);

    // ���� �߰��� �Լ�: ������ ���̺��� ������ ���̺긦 ����
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void RequestRandomSpawnWave();

protected:
    virtual void BeginPlay() override;

private:

    UFUNCTION() 
    void SpawnBoss();

    // ���� ������ ��ġ�� ã�� �Լ�
    bool FindSpawnLocation(FVector& OutLocation);

    // ����Ʈ/���� ��� ��, Ÿ�̸Ӹ� �̿��� ���� ���͸� �����ϴ� �Լ�
    void SpawnSingleMonster(const FMobSpawnInfo* SpawnInfo, const FVector& Location, float DifficultyMultiplier);

    void SpawnSingleBoss(const FBossSpawnInfo* BossInfo, const FVector& Location, float DifficultyMultiplier);


private:

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

};
