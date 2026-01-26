// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BRGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API ABRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABRGameModeBase();

public:
	virtual void OnPlayerDied(AController* Controller);

	void RestartCurrentLevel();
protected:
	virtual void BeginPlay() override;

private:
	void TickTimer();

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName GameLevelName;
	//FTimerHandle GameTimerHandle;
};
