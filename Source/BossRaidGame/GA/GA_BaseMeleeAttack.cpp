// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_BaseMeleeAttack.h"
#include "AT/AT_SphereTraceForTargets.h" // ���� ���� �½�ũ ��� ����
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Tag/BRGameplayTag.h"

UGA_BaseMeleeAttack::UGA_BaseMeleeAttack()
{
	// �����Ƽ�� �⺻���� �Ӽ� ����
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(BRTAG_CHARACTER_ISDEAD);

}

void UGA_BaseMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage || !DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	//HitActors.Empty();
	// ��Ÿ�� ��� �½�ũ ����
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_BaseMeleeAttack::OnCompleteCallback);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_BaseMeleeAttack::OnInterruptedCallback);

	// ������ �̺�Ʈ ��� �½�ũ ����
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);

	if (MontageTask && EventTask)
	{

		// ������ �̺�Ʈ �±װ� ������ OnDamageEvent �Լ� ȣ��
		EventTask->EventReceived.AddDynamic(this, &UGA_BaseMeleeAttack::OnDamageEvent);

		MontageTask->ReadyForActivation();
		EventTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
void UGA_BaseMeleeAttack::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);

}

void UGA_BaseMeleeAttack::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
void UGA_BaseMeleeAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	// �����Ƽ ��� �� ��ٿ� ���� ���� ����
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	// ��������Ʈ �ڵ� ����
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}
void UGA_BaseMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// �����Ƽ�� ��ҵ��� �ʾҴٸ� ��ٿ� ����
	if (!bWasCancelled && CurrentActorInfo)
	{
		//const UGameplayEffect* CooldownEffect = CooldownEffectClass->GetDefaultObject<UGameplayEffect>();

		//ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CooldownEffect, 1.0f);
	}
	// ��������Ʈ �ڵ� ����
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}

void UGA_BaseMeleeAttack::OnDamageEvent(FGameplayEventData Payload)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return;

	// --- ������ �浹 �˻� ������ �Ʒ� �½�ũ ȣ��� ��ü ---
	FVector WorldOffset = OwnerCharacter->GetActorTransform().TransformVector(AttackLocationOffset);

	const FVector TraceStart = OwnerCharacter->GetActorLocation() + WorldOffset;
	//const FVector TraceEnd = TraceStart;	// + (OwnerCharacter->GetActorForwardVector() * AttackRadius)


	// �浹 �˻� �½�ũ ���� �� Ȱ��ȭ
	//UAT_SphereTraceForTargets* TraceTask = UAT_SphereTraceForTargets::SphereTraceForTargets(
	//	this,
	//	TraceStart,
	//	TraceEnd,
	//	AttackRadius,
	//	true // ����� �ð�ȭ Ȱ��ȭ
	//	, TargetChannel
	//);
	UAT_SphereTraceForTargets* TraceTask = UAT_SphereTraceForTargets::SphereTraceForTargets(
		this,
		TraceStart, // ���� ��ġ�� ������ ����
		AttackRadius,
		TargetChannel
	);

	if (TraceTask)
	{
		TraceTask->OnTargetsFound.AddDynamic(this, &UGA_BaseMeleeAttack::OnTargetsHit);
		TraceTask->ReadyForActivation();
	}
}

void UGA_BaseMeleeAttack::OnTargetsHit(const TArray<AActor*>& HitActors)
{
	TArray<TWeakObjectPtr<AActor>> ActorsToApplyEffect;

	for (AActor* Character : HitActors)
	{
		if (Character && !ActorsToApplyEffect.Contains(Character))
		{
			ActorsToApplyEffect.Add(Character);
		}
	}

	if (ActorsToApplyEffect.Num() > 0)
	{
		//���� �迭�� ���� �� �ִ� TargetData ��ü�� ����
		FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
		NewTargetData->TargetActorArray = ActorsToApplyEffect;

		// TargetData ��ü�� TargetDataHandle�� ����
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(NewTargetData);

		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetDataHandle, DamageEffectClass, 1.0f);
	}
}
