// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/CharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Prop/DamageIndicatorActor.h"
#include "Tag/BRGameplayTag.h"

UCharacterAttributeSet::UCharacterAttributeSet() :
AttackPower(30.0f),
AttackSpeed(1.0f),
MoveSpeed(1.0f),
Health(100.0f),
MaxHealth(100.0f),
Damage(0.0f)
{
	InitHealth(GetMaxHealth());
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));

	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		

		UAbilitySystemComponent* MyASC = GetOwningAbilitySystemComponent();
		AActor* MyAvatar = MyASC ? MyASC->GetAvatarActor() : nullptr;

		if (MyAvatar && MyASC)
		{
			FGameplayCueParameters Params;
			Params.RawMagnitude = GetDamage();

			Params.Location = MyAvatar->GetActorLocation();

			Params.Location.X += FMath::FRandRange(-50.0f, 50.0f);
			Params.Location.Y += FMath::FRandRange(-50.0f, 50.0f);
			Params.Location.Z += FMath::FRandRange(50.0f, 70.0f);


			MyASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Damage")), Params);
		}
		SetDamage(0.0f);

	}
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast();
	}

	

	bOutOfHealth = (GetHealth() <= 0.0f);

}
