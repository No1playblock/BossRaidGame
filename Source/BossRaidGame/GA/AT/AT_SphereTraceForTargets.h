// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_SphereTraceForTargets.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const TArray<AActor*>&, HitResults);


UCLASS()
class BOSSRAIDGAME_API UAT_SphereTraceForTargets : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// 태스크가 성공적으로 타겟을 찾았을 때 호출될 델리게이트
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnTargetsFound;

	/**
	 * 구체 형태의 충돌 검사를 수행하는 태스크를 생성하고 활성화합니다.
	 * @param OwningAbility 태스크를 소유한 어빌리티
	 * @param StartLocation 추적 시작 위치
	 * @param EndLocation 추적 종료 위치
	 * @param Radius 추적 구체의 반지름
	 * @param bDebug 디버깅 시각화를 표시할지 여부
	 */
	/*UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_SphereTraceForTargets* SphereTraceForTargets(
		UGameplayAbility* OwningAbility,
		FVector StartLocation,
		FVector EndLocation,
		float Radius,
		bool bDebug = false
		, ECollisionChannel Channel = ECollisionChannel::ECC_Pawn
	);*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_SphereTraceForTargets* SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel);
private:
	
	// 태스크가 실제 로직을 수행하는 함수
	virtual void Activate() override;

	/*FVector TraceStart;
	FVector TraceEnd;
	float TraceRadius;
	bool bShowDebug;*/

	FVector SpherePos;
	float SphereRadius;
	ECollisionChannel TargetChannel;
};
