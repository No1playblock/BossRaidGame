// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/MobCharacterAttributeSet.h"

UMobCharacterAttributeSet::UMobCharacterAttributeSet() :
	ExpReward(0.0f)
{
	
}

void UMobCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UMobCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
