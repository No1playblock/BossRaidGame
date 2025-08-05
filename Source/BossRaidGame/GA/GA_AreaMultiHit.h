// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "GA_AreaMultiHit.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BOSSRAIDGAME_API UGA_AreaMultiHit : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_AreaMultiHit();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetHit(const TArray<FOverlapResult>& OverlapResults);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag DamageEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int32 NumberOfHits = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float DelayBetweenHits = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AreaRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UParticleSystem* AreaVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	USoundBase* AreaSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FName TargetLocationKeyName;
};
