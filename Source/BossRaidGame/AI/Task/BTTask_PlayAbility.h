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
	/** �����̺�� Ʈ�� �����Ϳ��� ������ �����Ƽ�� �±׸� �����մϴ�. */
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityTag;

	// �½�ũ�� ���� ����
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// �½�ũ�� �߰��� ��ҵ� �� ȣ��Ǵ� �Լ�
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// �����Ƽ�� ����Ǿ��� �� ȣ��� �ݹ� �Լ�
	UFUNCTION()
	void OnAbilityEnded(const FAbilityEndedData& EndedData);

	// ���� �½�ũ�� ������ BehaviorTreeComponent�� ĳ��
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
