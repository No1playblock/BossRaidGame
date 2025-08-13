// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChooseAttack.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAttackSelectionPolicy : uint8
{
	MeleeOnly,
	RangedOnly,
	All
};
//일반몹은 공격 하나, 보스는 여러개 이므로 보스만 해당되는 Task
UCLASS()
class BOSSRAIDGAME_API UBTTask_ChooseAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ChooseAttack();

protected:
	// 비헤이비어 트리 태스크의 메인 실행 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에디터에서 설정할 수 있도록 최소/최대 공격 인덱스 범위를 변수로 만듦
	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MinAttackIndex = 1;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MaxAttackIndex = 5;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector SelectedAttackTagKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	EAttackSelectionPolicy AttackPolicy;
};
