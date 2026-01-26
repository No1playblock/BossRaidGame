// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SphereTraceForTargets.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const TArray<FOverlapResult>&);


UCLASS()
class BOSSRAIDGAME_API UAT_SphereTraceForTargets : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// 태스크가 성공적으로 타겟을 찾았을 때 호출될 델리게이트
	FTraceResultDelegate OnTargetsFound;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_SphereTraceForTargets* SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel);
private:	

	virtual void Activate() override;


	FVector SpherePos;
	float SphereRadius;
	ECollisionChannel TargetChannel;
};
