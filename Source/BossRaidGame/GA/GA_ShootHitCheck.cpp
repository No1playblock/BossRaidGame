// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ShootHitCheck.h"
#include "AT/AT_LineTrace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "AT/AT_ShootProjectile.h"
#include "Character/BaseCharacter.h"
#include "AT/AT_ShootProjectile.h"

UGA_ShootHitCheck::UGA_ShootHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

}

void UGA_ShootHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	InvokeGameplayCue();

	UAT_ShootProjectile* ShootTask = UAT_ShootProjectile::CreateTask(this, FName("ShootTask"));
	ShootTask->Initialize(BulletClass, DamageEffectClass);

	ShootTask->ReadyForActivation();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}

void UGA_ShootHitCheck::InvokeGameplayCue()
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = GetAvatarActorFromActorInfo();
	Param.Location = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo())->GetMesh()->GetSocketLocation(TEXT("Socket_Barrel"));
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueShootTag, Param);
}