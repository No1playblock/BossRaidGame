#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "AttackData.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AttackTag;

	// 이 공격의 기본 데미지 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	FAttackData() : Damage(0.f) {}
};