// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_BaseMeleeAttack.h"
#include "AT/AT_SphereTraceForTargets.h" // 새로 만든 태스크 헤더 포함
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Tag/BRGameplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Attribute/CharacterAttributeSet.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"
#include "Engine/OverlapResult.h"


UGA_BaseMeleeAttack::UGA_BaseMeleeAttack()
{
	// 어빌리티의 기본적인 속성 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(BRTAG_CHARACTER_ISDEAD);
	ActivationBlockedTags.AddTag(CooldownTag);

	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // 기본적으로 GameTraceChannel4(BossAttackCollision) 사용
}

void UGA_BaseMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage || !DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get()))
	{
		float CooldownTime = 1.0f; // 기본 쿨타임

		// SkillTreeData로부터 Damage 값 추출
		if (USkillTreeComponent* SkillComp = Player->FindComponentByClass<USkillTreeComponent>())
		{
			const FSkillTreeDataRow* SkillData = SkillComp->FindSkillDataByGrantedAbility(GetClass());
			if (SkillData)
			{
				Damage = SkillData->SkillDamage;
				CooldownTime = SkillData->SkillCoolTime;
				//UE_LOG(LogTemp, Warning, TEXT("AutoRaser Damage: %f, CooldownTime: %f"), Damage, CooldownTime);
			}
		}
		if (CooldownEffectClass)
		{
			// 쿨다운 이펙트의 지속시간을 SkillData에서 가져온 값으로 설정하여 적용
			FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffectClass);
			CooldownSpecHandle.Data->SetDuration(CooldownTime, true);
			CooldownSpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
			UE_LOG(LogTemp, Warning, TEXT("Applied Cooldown Effect: %s with Duration: %f"), *CooldownEffectClass->GetName(), CooldownTime);
		}
	}
	else
	{
		Damage = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UCharacterAttributeSet::GetAttackPowerAttribute());
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

void UGA_BaseMeleeAttack::OnDamageEvent_Implementation(FGameplayEventData Payload)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return;

	FVector WorldOffset = OwnerCharacter->GetActorTransform().TransformVector(AttackLocationOffset);

	const FVector TraceStart = OwnerCharacter->GetActorLocation() + WorldOffset;
	

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

void UGA_BaseMeleeAttack::OnTargetsHit(const TArray<FOverlapResult>& OverlapResults)
{
	TArray<TWeakObjectPtr<AActor>> HitActors;
	TArray<AActor*> HitActorArray;
	AActor* OwnerActor = GetAvatarActorFromActorInfo();

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && HitActor != OwnerActor && !HitActors.Contains(HitActor))
		{
			HitActors.AddUnique(HitActor);

			IDamageDataProvider* DamageProvider = Cast<IDamageDataProvider>(GetAvatarActorFromActorInfo());
			if (DamageProvider)
			{
				//TODO:
				//AttackTag를 ABILITY_ATTACK_BOSS__NIGHTMARE_SWIPE로 정적으로 설정할지 고민
				//보스면 태그에 따라, 보스가 아니면 AttackPower를 사용
				/*보완 필요함 보스, 몹, 플레이어일 때 어떻게 할 것 인지*/
				/*보스는 Tag가 필요 몹은 AttackPower, 플레이어는 EAbilityInputID가 필요*/
				float BaseDamage = DamageProvider->GetDamageByAttackTag(AbilityTags.First());
				if (AbilityInputID!=EAbilityInputID::None)
				{
					AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(GetAvatarActorFromActorInfo());
					if (Player)
					{
						// 플레이어의 스킬 트리 컴포넌트에서 데미지 값을 가져옴
						if (USkillTreeComponent* SkillComp = Player->GetSkillTreeComponent())
						{
							const FSkillTreeDataRow* SkillData = SkillComp->FindAcquiredSkillByInputID(AbilityInputID);
							BaseDamage = SkillData ? SkillData->SkillDamage : BaseDamage;
						}
					}
				}
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
