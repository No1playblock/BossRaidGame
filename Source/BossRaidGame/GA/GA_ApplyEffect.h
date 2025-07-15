// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ApplyEffect.generated.h"

/**
 * 
 */
class AActor;

UCLASS()
class BOSSRAIDGAME_API UGA_ApplyEffect : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ApplyEffect();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void InvokeGameplayCue(const AActor* Statue);

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> EffectToApply;

	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueEffectTag;

};
