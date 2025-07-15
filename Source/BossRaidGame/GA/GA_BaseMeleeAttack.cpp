// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_BaseMeleeAttack.h"
#include "AT/AT_SphereTraceForTargets.h" // 새로 만든 태스크 헤더 포함
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

// ... 생성자, ActivateAbility, OnAbilityCancelled, OnAbilityEnded 함수는 이전과 거의 동일 ...
// ActivateAbility 함수는 그대로 두시면 됩니다.

UGA_BaseMeleeAttack::UGA_BaseMeleeAttack()
{
	// 어빌리티의 기본적인 속성 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
		// 태스크의 OnTargetsFound 델리게이트에 우리의 처리 함수를 바인딩
		TraceTask->OnTargetsFound.AddDynamic(this, &UGA_BaseMeleeAttack::OnTargetsHit);
		TraceTask->ReadyForActivation();
	}
}

void UGA_BaseMeleeAttack::OnTargetsHit(const TArray<AActor*>& HitActors)
{
	// 이전에 멤버 변수로 옮긴 HitActors Set을 사용합니다.
	TArray<TWeakObjectPtr<AActor>> ActorsToApplyEffect;

	// 먼저 피해를 줄 고유한 액터 목록을 만듭니다.
	for (AActor* Character : HitActors)
	{
		if (Character && !ActorsToApplyEffect.Contains(Character))
		{
			// 이 공격으로 처음 맞는 액터라면, 목록에 추가하고 HitActors Set에도 기록합니다.
			ActorsToApplyEffect.Add(Character);
		}
	}

	if (ActorsToApplyEffect.Num() > 0)
	{
		// 1. 액터 배열을 담을 수 있는 TargetData 객체를 생성합니다.
		FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
		NewTargetData->TargetActorArray = ActorsToApplyEffect;

		// 2. 위에서 만든 TargetData 객체를 TargetDataHandle '상자'에 넣습니다.
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(NewTargetData);

		// 3. 올바른 타입의 TargetDataHandle을 사용하여 이펙트를 한 번에 적용합니다.
		ApplyGameplayEffectToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetDataHandle, DamageEffectClass, 1.0f);
	}
}
