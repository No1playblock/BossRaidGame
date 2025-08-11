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

	// ���� ���� �� �ʱ� �ð� ���� (��: 300�� = 5��)
	ABRGameState* GS = GetGameState<ABRGameState>();
	if (GS)
	{
		GS->RemainingTime = 300;
		// OnRep �Լ��� ���������� ȣ���Ͽ� ȣ��Ʈ PC�� UI�� ������Ʈ�ǵ��� ��
		GS->BroadcastTimeChanged();
	}

	// 1�ʸ��� TickTimer �Լ��� �ݺ��ؼ� ȣ���ϴ� Ÿ�̸� ����
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
			// ���� �ð��� 1�ʾ� ����
			GS->RemainingTime--;
			// OnRep �Լ��� ���������� ȣ��
			GS->BroadcastTimeChanged();
		}
		else
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		}
	}
}