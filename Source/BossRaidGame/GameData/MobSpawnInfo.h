// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MobSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct FMobSpawnInfo : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 스폰될 몬스터 블루프린트
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ACharacter> MonsterClass;

    // 스폰할 몬스터 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SpawnCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UParticleSystem* SpawnVFX;

    // 스폰 시 사용할 사운드 (SFX)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* SpawnSFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float WalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float RunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float AttackPower;
};