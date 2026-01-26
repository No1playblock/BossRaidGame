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
	TargetChannel = ECollisionChannel::ECC_GameTraceChannel4; // 기본적으로 GameTraceChannel4(BossAttackCollision) 사용
}

void UGA_AreaMultiHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	if (MontageTask)
	{
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_AreaMultiHit::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DamageEventTag);
	if (EventTask)
	{
		// 이벤트가 수신되면 OnDamageEvent 함수 호출
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

	//현재는 AI 전용 어빌리티임.
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
		MultiHitTask->OnHit.AddUObject(this, &UGA_AreaMultiHit::OnTargetHit);
		MultiHitTask->OnFinished.AddUObject(this, &UGA_AreaMultiHit::OnMontageCompleted);

		MultiHitTask->ReadyForActivation();
	}
}
void UGA_AreaMultiHit::OnTargetHit(const TArray<FOverlapResult>& OverlapResults)
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
void UGA_AreaMultiHit::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_AreaMultiHit::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
