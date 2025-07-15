// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SphereTraceForTargets.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const TArray<FHitResult>&, HitResults);


UCLASS()
class BOSSRAIDGAME_API UAT_SphereTraceForTargets : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// �½�ũ�� ���������� Ÿ���� ã���� �� ȣ��� ��������Ʈ
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnTargetsFound;

	/**
	 * ��ü ������ �浹 �˻縦 �����ϴ� �½�ũ�� �����ϰ� Ȱ��ȭ�մϴ�.
	 * @param OwningAbility �½�ũ�� ������ �����Ƽ
	 * @param StartLocation ���� ���� ��ġ
	 * @param EndLocation ���� ���� ��ġ
	 * @param Radius ���� ��ü�� ������
	 * @param bDebug ����� �ð�ȭ�� ǥ������ ����
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_SphereTraceForTargets* SphereTraceForTargets(
		UGameplayAbility* OwningAbility,
		FVector StartLocation,
		FVector EndLocation,
		float Radius,
		bool bDebug = false
	);

private:
	// �½�ũ�� ���� ������ �����ϴ� �Լ�
	virtual void Activate() override;

	FVector TraceStart;
	FVector TraceEnd;
	float TraceRadius;
	bool bShowDebug;
};
