// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/OverlapResult.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AT_MultiHitAtLocation.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FMultiHitHitDelegate, const TArray<FOverlapResult>&);

DECLARE_MULTICAST_DELEGATE(FMultiHitFinishedDelegate);

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UAT_MultiHitAtLocation : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAT_MultiHitAtLocation* MultiHitAtLocation(
		UGameplayAbility* OwningAbility,
		FName TargetLocationKeyName, // FVector 대신 BlackboardKeySelector를 받음
		int32 NumberOfHits,
		float DelayBetweenHits,
		float AreaRadius,
		TEnumAsByte<ECollisionChannel> TargetChannel,
		UParticleSystem* AreaVFX, // VFX와 SFX를 태스크가 직접 받도록 추가
		USoundBase* AreaSFX
	);

	FMultiHitHitDelegate OnHit;

	FMultiHitFinishedDelegate OnFinished;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformNextHit();

	FName TargetLocationKeyName; 
	int32 NumberOfHits;
	float DelayBetweenHits;
	float AreaRadius;
	TEnumAsByte<ECollisionChannel> TargetChannel;
	UParticleSystem* AreaVFX;
	USoundBase* AreaSFX;

	int32 CurrentHitCount;
	FTimerHandle TimerHandle;
	TWeakObjectPtr<UBlackboardComponent> BlackboardComp; // 블랙보드 컴포넌트를 캐시

};
