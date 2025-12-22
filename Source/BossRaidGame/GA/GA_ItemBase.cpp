// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ItemBase.h"
#include "AbilitySystemComponent.h"

UGA_ItemBase::UGA_ItemBase()
{
	// AbilityInstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}
void UGA_ItemBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//설정된 GE가 있다면 나 자신에게 적용
	if (EffectToApply && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, EffectToApply);
		if (SpecHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_ItemBase: EffectToApply is NULL!"));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
