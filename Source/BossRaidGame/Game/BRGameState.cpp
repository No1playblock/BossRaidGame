// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameState.h"
#include "Net/UnrealNetwork.h"

void ABRGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// RemainingTime 변수를 네트워크를 통해 복제하도록 설정
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