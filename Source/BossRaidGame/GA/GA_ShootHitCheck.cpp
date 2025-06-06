// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ShootHitCheck.h"
#include "AT/AT_LineTrace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"

UGA_ShootHitCheck::UGA_ShootHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

}

void UGA_ShootHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	
	UAT_LineTrace* AttackTraceTask = UAT_LineTrace::CreateTask(this, FName("LineTraceTask"));
	//AttackTraceTask->OnComplete.AddDynamic(this, &UGA_ShootHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}

void UGA_ShootHitCheck::OnHitTarget(const FHitResult& Hit)
{
	AActor* Target = Hit.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("HitAcotr Name: %s"), *Target->GetFName().ToString());
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
