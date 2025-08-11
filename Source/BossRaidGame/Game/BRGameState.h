// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BRGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChangedDelegate, int32, NewTime);

UCLASS()
class BOSSRAIDGAME_API ABRGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnTimeChangedDelegate OnTimeChanged;

	// ���������� ���� �����ϰ�, Ŭ���̾�Ʈ������ �б⸸ �ϵ��� ����
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Game")
	int32 RemainingTime;

	void BroadcastTimeChanged();
protected:
	// RemainingTime ������ �������� ����Ǿ� Ŭ���̾�Ʈ�� ������ �� �ڵ����� ȣ��Ǵ� �Լ�
	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
