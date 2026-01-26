// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_SpawnActorAtCursor.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_SpawnActorAtCursor : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SpawnActorAtCursor();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    UFUNCTION()
    void OnGameplayEventReceived(FGameplayEventData Payload);

protected:

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TSubclassOf<AActor> SpawnActorClass;

    UPROPERTY(BlueprintReadWrite, Category = "GAS")
    TObjectPtr<AActor> SpawnedActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
    FVector SpawnOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Animation")
    TObjectPtr<UAnimMontage> MontageToPlay;

    // 적용할 쿨다운 이펙트 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Cooldown")
    TSubclassOf<UGameplayEffect> CooldownEffectClass;

    // 쿨다운 태그 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Cooldown")
    FGameplayTag CooldownTag;

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    float Damage = 0.0f;

};
