// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_ItemBase : public UGameplayAbility
{
	GENERATED_BODY()
	
	UGA_ItemBase();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 에디터에서 설정할 이펙트 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Effects")
	TSubclassOf<class UGameplayEffect> EffectToApply;
};
