// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_AttackDamage.h"
#include "Attribute/CharacterAttributeSet.h"
UGE_AttackDamage::UGE_AttackDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(UCharacterAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UCharacterAttributeSet, Health)));
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FScalableFloat DamageAmount(-30.0f);
	FGameplayEffectModifierMagnitude ModMagnitude(DamageAmount);

	HealthModifier.ModifierMagnitude = ModMagnitude;
	Modifiers.Add(HealthModifier);

}
