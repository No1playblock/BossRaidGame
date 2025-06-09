// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_ShootBase.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootHit, const FHitResult&, HitResult);


UENUM(BlueprintType)
enum class EShootType : uint8
{
	LineTrace  UMETA(DisplayName = "LineTrace"),
	Projectile UMETA(DisplayName = "Projectile"),
	Arc UMETA(DisplayName = "Arc")
};


UCLASS(Abstract)
class BOSSRAIDGAME_API UAT_ShootBase : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnShootHit OnHit;

	// 각 Task가 구현할 virtual 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))

	virtual UAT_ShootBase* CreateTask(UGameplayAbility* OwningAbility, FName TaskName) PURE_VIRTUAL(UAT_ShootBase::CreateTaskInstance, return nullptr;);

protected:
	virtual void Activate() override;
	virtual void DoShoot() PURE_VIRTUAL(UAT_ShootBase::DoShoot, );
	
};
