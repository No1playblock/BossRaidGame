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
void UMobCharacterAttributeSet::Reset()
{
	// 체력을 최대로 설정함
	SetHealth(GetMaxHealth());

	// 사망 상태 플래그를 false로 초기화함
	bOutOfHealth = false;
}