// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_ShootBase.h"
#include "AT_LineTrace.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineTraceHit, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAT_LineTrace : public UAT_ShootBase
{
	GENERATED_BODY()

public:
	UAT_LineTrace();
	virtual UAT_LineTrace* CreateTask(UGameplayAbility* OwningAbility, FName TaskInstanceName) override;

	/*UPROPERTY(BlueprintAssignable)
	FOnLineTraceHit OnHit;*/
protected:
	virtual void Activate() override;

	virtual void DoShoot() override;

};
