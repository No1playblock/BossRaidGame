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
	// �� �������̽��� �����ϴ� Ŭ������ �ݵ�� �� �Լ��� ������ ��.
	// Ư�� ���� �±׿� �ش��ϴ� ������ ���� ��ȯ
	virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const = 0;
};
