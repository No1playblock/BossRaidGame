// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ArcAttack.h"
#include "AT/AT_ArcSweepForTargets.h" // 새로 만든 태스크 헤더 포함
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"
#include "Tag/BRGameplayTag.h"
#include "Interface/DamageDataProvider.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"
UGA_ArcAttack::UGA_ArcAttack()
{
	// 어빌리티의 기본적인 속성 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // 기본적으로 GameTraceChannel4(BossAttackCollision) 사용
	ActivationBlockedTags.AddTag(CooldownTag);

}

void UGA_ArcAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage || !DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if(!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//만약에 플레이어에서 사용하는거라면 DT에서 받아와서 데미지와 스킬쿨타임을 설정하도록
	//나중에 이부분도 부모클래스나 따로 뺴야될듯.
	if(AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get()))
	{
		float Damage = 100.f;
		float CooldownTime = 1.0f; // 기본 쿨타임

		// SkillTreeData로부터 Damage 값 추출
		if (USkillTreeComponent* SkillComp = Player->FindComponentByClass<USkillTreeComponent>())
		{
			const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
			if (SkillData)
			{
				Damage = SkillData->SkillDamage;
				CooldownTime = SkillData->SkillCoolTime;
			}
		}
		if (CooldownEffectClass)
		{
			// 쿨다운 이펙트의 지속시간을 SkillData에서 가져온 값으로 설정하여 적용
			FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffectClass);
			CooldownSpecHandle.Data->SetDuration(CooldownTime, true);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
		}
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
		ArcTraceTask->OnTargetsFound.AddUObject(this, &UGA_ArcAttack::OnTargetsHit);
		ArcTraceTask->ReadyForActivation();
	}
}

void UGA_ArcAttack::OnTargetsHit(const TArray<FOverlapResult>& OverlapResults)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	TArray<TWeakObjectPtr<AActor>> HitActors;
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && HitActor != OwnerActor && !HitActors.Contains(HitActor))
		{
			HitActors.AddUnique(HitActor);

			IDamageDataProvider* DamageProvider = Cast<IDamageDataProvider>(GetAvatarActorFromActorInfo());
			if (DamageProvider)
			{
				//AttackTag를 ABILITY_ATTACK_BOSS__NIGHTMARE_SWIPE로 정적으로 설정할지 고민
				const float BaseDamage = DamageProvider->GetDamageByAttackTag(AbilityTags.First());

				// Target의 ASC 가져오기
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
				if (!TargetASC) continue;

				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, BaseDamage);

					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}

}
