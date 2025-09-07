// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ArcAttack.h"
#include "AT/AT_ArcSweepForTargets.h" // ���� ���� �½�ũ ��� ����
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
	// �����Ƽ�� �⺻���� �Ӽ� ����
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // �⺻������ GameTraceChannel4(BossAttackCollision) ���
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

	//���࿡ �÷��̾�� ����ϴ°Ŷ�� DT���� �޾ƿͼ� �������� ��ų��Ÿ���� �����ϵ���
	//���߿� �̺κе� �θ�Ŭ������ ���� ���ߵɵ�.
	if(AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(ActorInfo->AvatarActor.Get()))
	{
		float Damage = 100.f;
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
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
			//UE_LOG(LogTemp, Warning, TEXT("Applied Cooldown Effect: %s with Duration: %f"), *CooldownEffectClass->GetName(), CooldownTime);
		}
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
				//AttackTag�� ABILITY_ATTACK_BOSS__NIGHTMARE_SWIPE�� �������� �������� ���
				const float BaseDamage = DamageProvider->GetDamageByAttackTag(AbilityTags.First());

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
