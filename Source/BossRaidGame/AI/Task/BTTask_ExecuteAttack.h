// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ExecuteAttack.generated.h"

/**
 * 
 */
struct FAbilityEndedData;

UCLASS()
class BOSSRAIDGAME_API UBTTask_ExecuteAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_ExecuteAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& EndedData);

	FGameplayTag AttackAbilityTag;

	FDelegateHandle AbilityEndedDelegateHandle;

	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
