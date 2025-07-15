// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ApplyRandomEffect.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_ApplyRandomEffect : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGA_ApplyRandomEffect();

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    void InvokeGameplayCue(const AActor* Statue);

protected:

    // �������Ʈ���� ������ ���� ���� ȿ�� ���
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> BuffEffects;


    UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
    FGameplayTag GameplayCueEffectTag;
};
