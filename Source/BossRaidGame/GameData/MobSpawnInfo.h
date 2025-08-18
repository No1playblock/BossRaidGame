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
    // ������ ���� �������Ʈ
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ACharacter> MonsterClass;

    // ������ ���� ��
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SpawnCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UParticleSystem* SpawnVFX;

    // ���� �� ����� ���� (SFX)
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