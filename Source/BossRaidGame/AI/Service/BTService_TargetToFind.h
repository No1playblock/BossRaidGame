// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TargetToFind.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UBTService_TargetToFind : public UBTService
{
	GENERATED_BODY()
	

public:
	UBTService_TargetToFind();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	float Radius;

	UPROPERTY()
	class ABRAIController* CachedController;
	UPROPERTY()
	class ANonPlayerGASCharacter* CachedMobCharacter;
	UPROPERTY()
	class UBlackboardComponent* CachedBlackboard;
};
