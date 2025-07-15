// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ApplyEffect.h"
#include "AbilitySystemComponent.h"


UGA_ApplyEffect::UGA_ApplyEffect()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ApplyEffect::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 적용할 이펙트가 지정되어 있는지, 이벤트 데이터가 유효한지 확인합니다.
	if (EffectToApply && TriggerEventData)
	{
		const AActor* StatueActor = TriggerEventData->Target;

		// 이펙트 컨텍스트를 만들고 SourceObject로 석상을 지정합니다.
		FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(StatueActor);

		// 이펙트 스펙을 생성합니다.
		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			// 이펙트를 소유자(플레이어)에게 적용합니다.
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			InvokeGameplayCue(StatueActor);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}
void UGA_ApplyEffect::InvokeGameplayCue(const AActor* Statue)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = GetAvatarActorFromActorInfo();
	Param.Location = Statue->GetActorLocation();
	Param.Location.Z -= 125.0f; // 석상 위에 위치하도록 Z축 위치를 조정
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueEffectTag, Param);
}