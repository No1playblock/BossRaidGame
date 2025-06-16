// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ShootHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_ShootHitCheck : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ShootHitCheck();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	void InvokeGameplayCue();

protected:
	float CurrentLevel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueShootTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class APrimaryBullet> BulletClass;
};
