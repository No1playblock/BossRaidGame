// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "DamageDataProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageDataProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BOSSRAIDGAME_API IDamageDataProvider
{
	GENERATED_BODY()

public:
	// 이 인터페이스를 구현하는 클래스는 반드시 이 함수를 만들어야 함.
	// 특정 공격 태그에 해당하는 데미지 값을 반환
	virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const = 0;
};
