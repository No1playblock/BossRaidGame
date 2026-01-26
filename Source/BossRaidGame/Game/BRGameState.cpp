// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameState.h"
#include "Net/UnrealNetwork.h"

void ABRGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABRGameState, RemainingTime);
}

void ABRGameState::OnRep_RemainingTime()
{
	BroadcastTimeChanged();
}

void ABRGameState::BroadcastTimeChanged()
{
	OnTimeChanged.Broadcast(RemainingTime);
}