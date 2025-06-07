// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_LineTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineTraceHit, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAT_LineTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_LineTrace();
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_LineTrace* CreateTask(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	UPROPERTY(BlueprintAssignable)
	FOnLineTraceHit OnHit;
protected:
	virtual void Activate() override;

	

	void PerformTrace();

};
