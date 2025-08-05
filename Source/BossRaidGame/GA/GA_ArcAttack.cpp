// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ArcAttack.h"
#include "AT/AT_ArcSweepForTargets.h" // ���� ���� �½�ũ ��� ����
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"


UGA_ArcAttack::UGA_ArcAttack()
{
	// �����Ƽ�� �⺻���� �Ӽ� ����
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TargetChannel = ECollisionChannel::ECC_Pawn; // �⺻������ Pawn ä���� ���
}

void UGA_ArcAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	MontageTask->OnCompleted.AddDynamic(this, &UGA_ArcAttack::OnCompleteCallback);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_ArcAttack::OnInterruptedCallback);

	// ������ �̺�Ʈ ��� �½�ũ ����
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);

	if (MontageTask && EventTask)
	{

		// ������ �̺�Ʈ �±װ� ������ OnDamageEvent �Լ� ȣ��
		EventTask->EventReceived.AddDynamic(this, &UGA_ArcAttack::OnDamageEvent);

		MontageTask->ReadyForActivation();
		EventTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
void UGA_ArcAttack::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);

}

void UGA_ArcAttack::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
void UGA_ArcAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	// �����Ƽ ��� �� ��ٿ� ���� ���� ����
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	
	// ��������Ʈ �ڵ� ����
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}
void UGA_ArcAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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

void UGA_ArcAttack::OnDamageEvent(FGameplayEventData Payload)
{
	// �浹 �˻� �½�ũ�� AT_ArcSweepForTargets�� ��ü
	UAT_ArcSweepForTargets* ArcTraceTask = UAT_ArcSweepForTargets::ArcSweepForTargets(
		this,
		ArcCenterOffset,
		ArcRadius,
		SphereRadius,
		SphereNum,
		ArcStartYaw,
		bIsClockwise,
		DelayBetweenSpheres, // << ���� �߰��� ���� ����
		TargetChannel,
		true
	);

	if (ArcTraceTask)
	{
		ArcTraceTask->OnTargetsFound.AddDynamic(this, &UGA_ArcAttack::OnTargetsHit);
		ArcTraceTask->ReadyForActivation();
	}
}

void UGA_ArcAttack::OnTargetsHit(const TArray<FOverlapResult>& OverlapResults)
{
	TArray<TWeakObjectPtr<AActor>> ActorsToApplyEffect;

	// FHitResult �迭���� ���͸� ����
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && !ActorsToApplyEffect.Contains(HitActor))
		{
			ActorsToApplyEffect.Add(HitActor);
		}
	}

	if (ActorsToApplyEffect.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
		NewTargetData->TargetActorArray = ActorsToApplyEffect;

		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(NewTargetData);

		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetDataHandle, DamageEffectClass, 1.0f);
	}
}
