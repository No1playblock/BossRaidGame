// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LevelStatData.generated.h"

USTRUCT(BlueprintType)
struct FLevelStatRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackPower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SkillPower;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SkillCooldownRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed;
};