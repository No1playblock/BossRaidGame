// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BRAIController.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API ABRAIController : public AAIController
{
	GENERATED_BODY()
	

public:
public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() { return BlackboardComp; }
	ABRAIController();
private:
	
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComp;

};
