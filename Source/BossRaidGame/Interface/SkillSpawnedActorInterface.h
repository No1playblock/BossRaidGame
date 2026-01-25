// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayEffect.h" 
#include "SkillSpawnedActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USkillSpawnedActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BOSSRAIDGAME_API ISkillSpawnedActorInterface

{
	GENERATED_BODY()

public:

	//초기화
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GAS")
	void InitializeInfo(float Damage, TSubclassOf<UGameplayEffect> DamageEffect);

	
	//발동
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GAS")
	void TriggerSkill();
};
