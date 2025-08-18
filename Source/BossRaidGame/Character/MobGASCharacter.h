// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/NonPlayerGASCharacter.h"
#include "MobGASCharacter.generated.h"

/**
 * 
 */
struct FMobSpawnInfo;

UCLASS()
class BOSSRAIDGAME_API AMobGASCharacter : public ANonPlayerGASCharacter
{
	GENERATED_BODY()
	
public:
	AMobGASCharacter();

	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }

	void InitializeFromData(const FMobSpawnInfo* MobData);


	void SetChasingState();
	void SetWalkingState();

protected:
	
	virtual void PossessedBy(AController* NewController) override;


	virtual void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> MovementStateEffectClass;

	float WalkSpeed;

	float RunSpeed;
};
