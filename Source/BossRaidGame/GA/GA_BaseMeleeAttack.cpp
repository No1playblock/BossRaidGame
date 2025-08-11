// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_BaseMeleeAttack.h"
#include "AT/AT_SphereTraceForTargets.h" // 새로 만든 태스크 헤더 포함
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Tag/BRGameplayTag.h"

UGA_BaseMeleeAttack::UGA_BaseMeleeAttack()
{
	// 어빌리티의 기본적인 속성 설정
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
	// 몽타주 재생 태스크 생성
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_BaseMeleeAttack::OnCompleteCallback);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_BaseMeleeAttack::OnInterruptedCallback);

	// 데미지 이벤트 대기 태스크 생성
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);

	if (MontageTask && EventTask)
	{

		// 데미지 이벤트 태그가 들어오면 OnDamageEvent 함수 호출
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
	// 어빌리티 취소 시 쿨다운 적용 없이 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	// 델리게이트 핸들 정리
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}
void UGA_BaseMeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// 어빌리티가 취소되지 않았다면 쿨다운 적용
	if (!bWasCancelled && CurrentActorInfo)
	{
		//const UGameplayEffect* CooldownEffect = CooldownEffectClass->GetDefaultObject<UGameplayEffect>();

		//ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, CooldownEffect, 1.0f);
	}
	// 델리게이트 핸들 정리
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}

void UGA_BaseMeleeAttack::OnDamageEvent(FGameplayEventData Payload)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return;

	// --- 기존의 충돌 검사 로직을 아래 태스크 호출로 대체 ---
	FVector WorldOffset = OwnerCharacter->GetActorTransform().TransformVector(AttackLocationOffset);

	const FVector TraceStart = OwnerCharacter->GetActorLocation() + WorldOffset;
	//const FVector TraceEnd = TraceStart;	// + (OwnerCharacter->GetActorForwardVector() * AttackRadius)


	// 충돌 검사 태스크 생성 및 활성화
	//UAT_SphereTraceForTargets* TraceTask = UAT_SphereTraceForTargets::SphereTraceForTargets(
	//	this,
	//	TraceStart,
	//	TraceEnd,
	//	AttackRadius,
	//	true // 디버깅 시각화 활성화
	//	, TargetChannel
	//);
	UAT_SphereTraceForTargets* TraceTask = UAT_SphereTraceForTargets::SphereTraceForTargets(
		this,
		TraceStart, // 공격 위치에 오프셋 적용
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
		//액터 배열을 담을 수 있는 TargetData 객체를 생성
		FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
		NewTargetData->TargetActorArray = ActorsToApplyEffect;

		// TargetData 객체를 TargetDataHandle에 넣음
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(NewTargetData);

		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetDataHandle, DamageEffectClass, 1.0f);
	}
}
