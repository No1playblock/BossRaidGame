// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerState.h"
#include "AbilitySystemComponent.h"

ABRPlayerState::ABRPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	//ASC->SetIsReplicated(true);
}

UAbilitySystemComponent* ABRPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
