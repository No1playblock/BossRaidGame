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
	
	//ABRAIController();

	ABRAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() { return BlackboardComp; }
protected:

	void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SeperationWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AvoidanceRangeMultiplier = 1.1f;
private:
	
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComp;



};
