// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_BaseMeleeAttack.h"
#include "AT/AT_SphereTraceForTargets.h" // ���� ���� �½�ũ ��� ����
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
	// �����Ƽ�� �⺻���� �Ӽ� ����
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(BRTAG_CHARACTER_ISDEAD);
	ActivationBlockedTags.AddTag(CooldownTag);

	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // �⺻������ GameTraceChannel4(BossAttackCollision) ���
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
		float CooldownTime = 1.0f; // �⺻ ��Ÿ��

		// SkillTreeData�κ��� Damage �� ����
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
			// ��ٿ� ����Ʈ�� ���ӽð��� SkillData���� ������ ������ �����Ͽ� ����
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

	FVector WorldOffset = OwnerCharacter->GetActorTransform().TransformVector(AttackLocationOffset);

	const FVector TraceStart = OwnerCharacter->GetActorLocation() + WorldOffset;
	

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
				//AttackTag�� ABILITY_ATTACK_BOSS__NIGHTMARE_SWIPE�� �������� �������� ���
				//������ �±׿� ����, ������ �ƴϸ� AttackPower�� ���
				/*���� �ʿ��� ����, ��, �÷��̾��� �� ��� �� �� ����*/
				/*������ Tag�� �ʿ� ���� AttackPower, �÷��̾�� EAbilityInputID�� �ʿ�*/
				float BaseDamage = DamageProvider->GetDamageByAttackTag(AbilityTags.First());
				if (AbilityInputID!=EAbilityInputID::None)
				{
					AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(GetAvatarActorFromActorInfo());
					if (Player)
					{
						// �÷��̾��� ��ų Ʈ�� ������Ʈ���� ������ ���� ������
						if (USkillTreeComponent* SkillComp = Player->GetSkillTreeComponent())
						{
							const FSkillTreeDataRow* SkillData = SkillComp->FindAcquiredSkillByInputID(AbilityInputID);
							BaseDamage = SkillData ? SkillData->SkillDamage : BaseDamage;
						}
					}
				}
				// Target�� ASC ��������
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
