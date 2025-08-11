// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SmoothFaceTarget.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UBTTask_SmoothFaceTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UBTTask_SmoothFaceTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float RotationSpeed = 5.0f;

	//목표 각도와 이 값 이하로 차이가 나면 회전을 완료한 것으로 간주
	UPROPERTY(EditAnywhere, Category = "AI")
	float Precision = 2.0f;
};
