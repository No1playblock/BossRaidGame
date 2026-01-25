// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Dash.generated.h"

/**
 * 
 */
class UNiagaraSystem;
class UAnimMontage;

UCLASS()
class BOSSRAIDGAME_API UGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Dash();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnDashFinished();

	void SpawnGhostTrail();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> FlashStepSystem;

	// 순간이동 느낌을 위해 힘은 세게, 시간은 짧게 설정
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float DashStrength = 20000.0f;


	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float DashDuration = 0.2f; 

	//잔상 생성간격
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float GhostSpawnInterval = 0.02f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TObjectPtr<UAnimMontage> DashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	// 쿨다운 태그 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TSubclassOf<class AGhostTrailActor> GhostActorClass;

private:
	// 타이머 관리용 핸들
	FTimerHandle GhostTimerHandle;
};
