// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_AttackHitCheck.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "AT/AT_CollisionSweep.h"

UGA_AttackHitCheck::UGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AGASCharacterPlayer* Character = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get());

	if (Character)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		ASC->GenericGameplayEventCallbacks.FindOrAdd(TriggerEndTag)
			.AddUObject(this, &UGA_AttackHitCheck::OnEndEventReceived);
	}

	Task = UAT_CollisionSweep::CreateTask(this, FName("SweepTask"));
	Task->ReadyForActivation();
	
	
	UE_LOG(LogTemp, Warning, TEXT("UGA_AttakcHitCHeck"));
}

void UGA_AttackHitCheck::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(TriggerEndTag)
			.RemoveAll(this); // 바인딩 해제
	}
	Task->OnDestroy(true);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End"));

}

void UGA_AttackHitCheck::OnEndEventReceived(const FGameplayEventData* Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
