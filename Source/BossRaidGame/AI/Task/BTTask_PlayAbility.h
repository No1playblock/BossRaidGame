// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_PlayAbility.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UBTTask_PlayAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAbility();

protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityTag;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& EndedData);

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
