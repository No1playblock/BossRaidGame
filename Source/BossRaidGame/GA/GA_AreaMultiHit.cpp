// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_AreaMultiHit.h"
#include "AT/AT_MultiHitAtLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h" 
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Tag/BRGameplayTag.h"
#include "Interface/DamageDataProvider.h"
#include "AbilitySystemBlueprintLibrary.h"
UGA_AreaMultiHit::UGA_AreaMultiHit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // �⺻������ GameTraceChannel4(BossAttackCollision) ���
}

void UGA_AreaMultiHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	if (MontageTask)
	{
		//MontageTask->OnCompleted.AddDynamic(this, &UGA_AreaMultiHit::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_AreaMultiHit::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);
	if (EventTask)
	{
		// �̺�Ʈ�� ���ŵǸ� OnDamageEvent �Լ� ȣ��
		EventTask->EventReceived.AddDynamic(this, &UGA_AreaMultiHit::OnDamageEvent);
		EventTask->ReadyForActivation();
	}

	if (!MontageTask || !EventTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
void UGA_AreaMultiHit::OnDamageEvent(FGameplayEventData Payload)
{
	AController* Controller = GetCurrentActorInfo()->PlayerController.IsValid() ? GetCurrentActorInfo()->PlayerController.Get() : GetCurrentActorInfo()->OwnerActor->GetInstigatorController();
	AAIController* AIController = Cast<AAIController>(Controller);
	if (!AIController) return;

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	AActor* TargetActor = Blackboard ? Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target"))) : nullptr;
	if (!TargetActor) return;

	UAT_MultiHitAtLocation* MultiHitTask = UAT_MultiHitAtLocation::MultiHitAtLocation(
		this,
		TargetLocationKeyName,
		NumberOfHits,
		DelayBetweenHits,
		AreaRadius,
		TargetChannel,
		AreaVFX,
		AreaSFX
	);

	if (MultiHitTask)
	{
		MultiHitTask->OnHit.AddDynamic(this, &UGA_AreaMultiHit::OnTargetHit);
		MultiHitTask->OnFinished.AddDynamic(this, &UGA_AreaMultiHit::OnMontageCompleted);

		MultiHitTask->ReadyForActivation();
	}
}
void UGA_AreaMultiHit::OnTargetHit(const TArray<FOverlapResult>& OverlapResults)
{
	/*TArray<TWeakObjectPtr<AActor>> HitActors;
	AActor* OwnerActor = GetAvatarActorFromActorInfo();

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && HitActor != OwnerActor && !HitActors.Contains(HitActor))
		{
			HitActors.AddUnique(HitActor);
		}
	}

	if (HitActors.Num() > 0 && DamageEffectClass)
	{
		FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
		NewTargetData->TargetActorArray = HitActors;
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		TargetDataHandle.Add(NewTargetData);

		ApplyGameplayEffectToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), TargetDataHandle, DamageEffectClass, 1.0f);
	}*/
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

					UE_LOG(LogTemp, Warning, TEXT("GA_AreaMultiHit::OnTargetsHit - DamageValue: %f"), BaseDamage);
					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}

}
void UGA_AreaMultiHit::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_AreaMultiHit::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
