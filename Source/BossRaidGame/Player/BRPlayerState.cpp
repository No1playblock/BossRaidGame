// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CharacterAttributeSet.h"

ABRPlayerState::ABRPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	//ASC->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ABRPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}