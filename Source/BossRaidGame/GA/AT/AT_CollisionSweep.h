// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_CollisionSweep.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAT_CollisionSweep : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAT_CollisionSweep();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_CollisionSweep* CreateTask(UGameplayAbility* OwningAbility, FName TaskInstanceName);
	
	virtual void OnDestroy(bool AbilityEnded) override;
protected:
	virtual void Activate() override;
	
	
	void PerformSweep();

protected:
	FTimerHandle SweepTimerHandle;


};
