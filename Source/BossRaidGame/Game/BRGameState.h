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

	// 서버에서만 값을 변경하고, 클라이언트에서는 읽기만 하도록 설정
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Game")
	int32 RemainingTime;

	void BroadcastTimeChanged();
protected:
	// RemainingTime 변수가 서버에서 변경되어 클라이언트로 복제될 때 자동으로 호출되는 함수
	UFUNCTION()
	void OnRep_RemainingTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
