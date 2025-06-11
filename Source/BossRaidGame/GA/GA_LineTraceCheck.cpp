// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_LineTraceCheck.h"
#include "AT/AT_LineTrace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "AT/AT_ShootBase.h"
#include "Character/BaseCharacter.h"
#include "AT/AT_LineTrace.h"

UGA_LineTraceCheck::UGA_LineTraceCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

}

void UGA_LineTraceCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	InvokeGameplayCue();

	UAT_LineTrace* AttackTraceTask = UAT_LineTrace::CreateTask(this, FName("LineTraceTask"));
	AttackTraceTask->OnHit.AddDynamic(this, &UGA_LineTraceCheck::OnHitTarget);
	AttackTraceTask->ReadyForActivation();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}

void UGA_LineTraceCheck::OnHitTarget(const FHitResult& Hit)
{
	AActor* Target = Hit.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("OntHitActor Name: %s"), *Target->GetFName().ToString());
	if (!Target || Target == GetAvatarActorFromActorInfo()) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

	if (TargetASC && SourceASC && DamageEffectClass)
	{
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContext);

		if (SpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
void UGA_LineTraceCheck::InvokeGameplayCue()
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = GetAvatarActorFromActorInfo();
	Param.Location = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo())->GetMesh()->GetSocketLocation(TEXT("Socket_Barrel"));
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueShootTag, Param);
}
