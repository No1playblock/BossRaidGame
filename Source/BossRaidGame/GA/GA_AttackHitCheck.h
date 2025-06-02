// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AttackHitCheck();

	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const  { return DamageEffectClass; }
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnEndEventReceived(const FGameplayEventData* Payload);

	UFUNCTION()
	void OnHitResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
protected:
	
	TObjectPtr<class UAT_CollisionSweep> Task;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerStartTag;

	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerEndTag;
};
