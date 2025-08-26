// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameData/MobSpawnInfo.h" // FMonsterSpawnInfo 구조체
#include "GameData/BossSpawnInfo.h" // FBossSpawnInfo 구조체
#include "NonPoolingMobSpawnManager.generated.h"

class UGameplayEffect;

UCLASS()
class BOSSRAIDGAME_API ANonPoolingMobSpawnManager : public AActor
{
	GENERATED_BODY()
public:
    ANonPoolingMobSpawnManager();

    // 데이터 테이블의 한 행(Row)을 기반으로 몬스터 웨이브 스폰을 요청하는 함수
    void RequestSpawnWave(const FName& WaveDataRowName);

    // 새로 추가된 함수: 데이터 테이블에서 랜덤한 웨이브를 스폰
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void ChooseRandomMob();

protected:
    virtual void BeginPlay() override;

private:

    UFUNCTION()
    void SpawnBoss();

    // 스폰 가능한 위치를 찾는 함수
    bool FindSpawnLocation(TSubclassOf<AActor> ActorClass, FVector& OutLocation);

    // 이펙트/사운드 재생 후, 타이머를 이용해 단일 몬스터를 스폰하는 함수
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


};
