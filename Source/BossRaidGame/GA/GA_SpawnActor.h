// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SpawnActor.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_SpawnActor : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SpawnActor();

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> SpawnActorClass;

	/** 기준 위치로부터의 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	FVector SpawnOffset;

	/** 데미지 적용을 위한 GE  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	/** 쿨타임 적용용 GE (스킬 트리 시간 적용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
	TSubclassOf<class UGameplayEffect> CooldownEffectClass;

	/** 쿨타임 태그  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

	/** 스킬 사용 시 재생할 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> MontageToPlay;
};
