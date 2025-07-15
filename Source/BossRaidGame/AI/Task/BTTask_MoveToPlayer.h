// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTTask_MoveToPlayer.generated.h"

/**
 * 
 */


UCLASS()
class BOSSRAIDGAME_API UBTTask_MoveToPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
    UBTTask_MoveToPlayer();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AcceptanceRadius = 100.f;

    UBehaviorTreeComponent* CachedOwnerComp = nullptr;

    FAIRequestID MoveRequestID;
};
