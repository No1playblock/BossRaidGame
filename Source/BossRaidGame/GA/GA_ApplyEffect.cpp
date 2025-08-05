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

	// ������ ����Ʈ�� �����Ǿ� �ִ���, �̺�Ʈ �����Ͱ� ��ȿ���� Ȯ��
	if (EffectToApply && TriggerEventData)
	{
		const AActor* StatueActor = TriggerEventData->Target;

		// ����Ʈ ���ؽ�Ʈ�� ����� SourceObject�� ������ ����
		FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(StatueActor);

		// ����Ʈ ������ ����
		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			// ����Ʈ�� �����ڿ��� ����
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
	Param.Location.Z -= 125.0f; // ���� ���� ��ġ�ϵ��� Z�� ��ġ�� ����
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueEffectTag, Param);
}