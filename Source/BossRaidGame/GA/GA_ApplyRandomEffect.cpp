// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_ApplyRandomEffect.h"
#include "AbilitySystemComponent.h"
UGA_ApplyRandomEffect::UGA_ApplyRandomEffect()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ApplyRandomEffect::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("Random Effect Ability Activated Successfully!"));
	// ����Ʈ �迭�� ������� ������, �̺�Ʈ �����Ͱ� ��ȿ���� Ȯ��
	if (BuffEffects.Num() > 0 && TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Applying Random Effect..."));
		// ���� ����Ʈ ����
		const int32 RandomIndex = FMath::RandRange(0, BuffEffects.Num() - 1);
		TSubclassOf<UGameplayEffect> EffectToApply = BuffEffects[RandomIndex];

		if (EffectToApply)
		{
			UE_LOG(LogTemp, Warning, TEXT("Effect Selected: %s"), *EffectToApply->GetName());
			const AActor* StatueActor = TriggerEventData->Target;

			FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(StatueActor);

			FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
				InvokeGameplayCue(StatueActor);

			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
void UGA_ApplyRandomEffect::InvokeGameplayCue(const AActor* Statue)
{
	UE_LOG(LogTemp, Warning, TEXT("Invoking Gameplay Cue for Statue: %s"), *Statue->GetName());
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = GetAvatarActorFromActorInfo();
	Param.Location = Statue->GetActorLocation();
	Param.Location.Z -= 125.0f; // ���� ���� ��ġ�ϵ��� Z�� ��ġ�� ����
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueEffectTag, Param);
}