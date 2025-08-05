// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ArcAttack.h"
#include "AT/AT_ArcSweepForTargets.h" // 새로 만든 태스크 헤더 포함
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"


UGA_ArcAttack::UGA_ArcAttack()
{
	// 어빌리티의 기본적인 속성 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TargetChannel = ECollisionChannel::ECC_Pawn; // 기본적으로 Pawn 채널을 사용
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
	// 몽타주 재생 태스크 생성
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_ArcAttack::OnCompleteCallback);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_ArcAttack::OnInterruptedCallback);

	// 데미지 이벤트 대기 태스크 생성
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);

	if (MontageTask && EventTask)
	{

		// 데미지 이벤트 태그가 들어오면 OnDamageEvent 함수 호출
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
	// 어빌리티 취소 시 쿨다운 적용 없이 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	
	// 델리게이트 핸들 정리
	OnGameplayAbilityEnded.Remove(EndedHandle);
	OnGameplayAbilityCancelled.Remove(CancelledHandle);
}
void UGA_ArcAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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

void UGA_ArcAttack::OnDamageEvent(FGameplayEventData Payload)
{
	// 충돌 검사 태스크를 AT_ArcSweepForTargets로 교체
	UAT_ArcSweepForTargets* ArcTraceTask = UAT_ArcSweepForTargets::ArcSweepForTargets(
		this,
		ArcCenterOffset,
		ArcRadius,
		SphereRadius,
		SphereNum,
		ArcStartYaw,
		bIsClockwise,
		DelayBetweenSpheres, // << 새로 추가된 변수 전달
		TargetChannel,
		true
	);

	if (ArcTraceTask)
	{
		// 태스크의 OnTargetsFound 델리게이트에 우리의 처리 함수를 바인딩
		ArcTraceTask->OnTargetsFound.AddDynamic(this, &UGA_ArcAttack::OnTargetsHit);
		ArcTraceTask->ReadyForActivation();
	}
}

void UGA_ArcAttack::OnTargetsHit(const TArray<FOverlapResult>& OverlapResults)
{
	TArray<TWeakObjectPtr<AActor>> ActorsToApplyEffect;

	// FHitResult 배열에서 액터를 추출
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
