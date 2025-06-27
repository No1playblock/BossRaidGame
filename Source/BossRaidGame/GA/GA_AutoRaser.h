// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AutoRaser.generated.h"

/**
 *
 */
UCLASS()
class BOSSRAIDGAME_API UGA_AutoRaser : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AutoRaser();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AutoRaser")
	TSubclassOf<class ARaserOrb> OrbActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "AutoRaser")
	float OrbSpawnRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "AutoRaser")
	float FireDelay = 0.5f;

	void SpawnOrbsAndFire(ACharacter* OwnerCharacter, float Damage);

	UPROPERTY(EditAnywhere)
	float OrbCount;
};
