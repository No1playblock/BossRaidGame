// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Shoot.h"
#include "Character/BaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GA/GA_Shoot.h"
#include "AbilitySystemComponent.h"
#include "Character/GASCharacterPlayer.h"
#include "Character/ComboActionData.h"
#include "GameFramework/Actor.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
UGA_Shoot::UGA_Shoot() :
	AnimationRate(1.0f)
{
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	CurrentComboData = Character->GetComboActionData();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	//Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	AnimationRate = ASC->GetNumericAttribute(UPlayerCharacterAttributeSet::GetAttackSpeedAttribute());

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), Character->GetAttackMontage(), AnimationRate);
	PlayAttackTask->OnCompleted.AddDynamic(this, &UGA_Shoot::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UGA_Shoot::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	//StartComboTimer();
}

void UGA_Shoot::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboInput = false;
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UGA_Shoot::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

}

void UGA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurrentComboData = nullptr;
	CurrentCombo = 0;
	HasNextComboInput = false;
}

void UGA_Shoot::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);

}

void UGA_Shoot::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}


