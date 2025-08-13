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
//�Ϲݸ��� ���� �ϳ�, ������ ������ �̹Ƿ� ������ �ش�Ǵ� Task
UCLASS()
class BOSSRAIDGAME_API UBTTask_ChooseAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ChooseAttack();

protected:
	// �����̺�� Ʈ�� �½�ũ�� ���� ���� �Լ�
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// �����Ϳ��� ������ �� �ֵ��� �ּ�/�ִ� ���� �ε��� ������ ������ ����
	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MinAttackIndex = 1;

	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MaxAttackIndex = 5;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector SelectedAttackTagKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	EAttackSelectionPolicy AttackPolicy;
};
