// Fill out your copyright notice in the Description page of Project Settings.


#include "BRGameModeBase.h"
#include "BRGameState.h"
ABRGameModeBase::ABRGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> CharacterRef(TEXT("/Game/Blueprints/Character/BP_CharacterPlayer.BP_CharacterPlayer_C"));
	if (CharacterRef.Class)
	{
		DefaultPawnClass = CharacterRef.Class;
	}
}
void ABRGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 초기 시간 설정 (예: 300초 = 5분)
	ABRGameState* GS = GetGameState<ABRGameState>();
	if (GS)
	{
		GS->RemainingTime = 300;
		// OnRep 함수를 서버에서도 호출하여 호스트 PC의 UI도 업데이트되도록 함
		GS->BroadcastTimeChanged();
	}

	// 1초마다 TickTimer 함수를 반복해서 호출하는 타이머 설정
	FTimerHandle GameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ABRGameModeBase::TickTimer, 1.0f, true);
}

void ABRGameModeBase::TickTimer()
{
	ABRGameState* GS = GetGameState<ABRGameState>();
	if (GS)
	{
		if (GS->RemainingTime > 0)
		{
			// 남은 시간을 1초씩 줄임
			GS->RemainingTime--;
			// OnRep 함수를 서버에서도 호출
			GS->BroadcastTimeChanged();
		}
		else
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		}
	}
}