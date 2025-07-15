// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_ShootProjectile.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAT_ShootProjectile : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))

	static UAT_ShootProjectile* CreateTask(UGameplayAbility* OwningAbility, FName TaskName);

	virtual void Activate() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APrimaryBullet> BulletClass;

	void Initialize(TSubclassOf<class APrimaryBullet> InBulletClass);
	
protected:

	void DoShoot();
};
