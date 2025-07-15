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
	/** 비헤이비어 트리 에디터에서 실행할 어빌리티의 태그를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityTag;

	// 태스크의 메인 로직
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 태스크가 중간에 취소될 때 호출되는 함수
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 어빌리티가 종료되었을 때 호출될 콜백 함수
	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& EndedData);

	// 현재 태스크를 실행한 BehaviorTreeComponent의 캐시
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
