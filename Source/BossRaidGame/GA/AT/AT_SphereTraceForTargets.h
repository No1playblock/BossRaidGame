// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SphereTraceForTargets.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const TArray<FOverlapResult>&, HitResults);


UCLASS()
class BOSSRAIDGAME_API UAT_SphereTraceForTargets : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// �½�ũ�� ���������� Ÿ���� ã���� �� ȣ��� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnTargetsFound;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_SphereTraceForTargets* SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel);
private:
	

	virtual void Activate() override;


	FVector SpherePos;
	float SphereRadius;
	ECollisionChannel TargetChannel;
};
