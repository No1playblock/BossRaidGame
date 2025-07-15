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
    // �������Ʈ���� ������ ���� ���� ���� Ŭ����
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
    TSubclassOf<AActor> SpawnActorClass;
};
