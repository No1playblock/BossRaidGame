// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_ArcSweepForTargets.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArcSweepDelegate, const TArray<FOverlapResult>&, OverlapResults);

UCLASS()
class BOSSRAIDGAME_API UAT_ArcSweepForTargets : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Arc Sweep For Targets", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_ArcSweepForTargets* ArcSweepForTargets(
		UGameplayAbility* OwningAbility,
		FVector ArcCenterOffset,
		float ArcRadius,
		float SphereRadius,
		int32 SphereNum,
		float ArcStartYaw,
		bool bIsClockwise,
		float DelayBetweenSpheres, // << ���� �߰��� �Ķ����
		ECollisionChannel TraceChannel,
		bool bDrawDebug
	);

	UPROPERTY(BlueprintAssignable)
	FArcSweepDelegate OnTargetsFound;

protected:
	// �½�ũ�� ������ Ȱ��ȭ�� �� ȣ��Ǵ� �Լ��Դϴ�.
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

private:

	void PerformNextSphereTrace();

	// �������Ʈ���� ���� �Է°����� ������ ������
	FVector ArcCenterOffset;
	float ArcRadius;
	float SphereRadius;
	int32 SphereNum;
	float ArcStartYaw;
	bool bIsClockwise;
	float DelayBetweenSpheres;
	ECollisionChannel TraceChannel;
	bool bDrawDebug;

	int32 CurrentSphereIndex;
	FTimerHandle TimerHandle;
	TArray<FOverlapResult> CombinedOverlapResults;
	TSet<AActor*> UniqueHitActors;
};
