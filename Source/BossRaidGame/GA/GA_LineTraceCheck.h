// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LineTraceCheck.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_LineTraceCheck : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_LineTraceCheck();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UFUNCTION()
	void OnHitTarget(const FHitResult& Hit);

	void InvokeGameplayCue();

protected:
	float CurrentLevel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueShootTag;
};
