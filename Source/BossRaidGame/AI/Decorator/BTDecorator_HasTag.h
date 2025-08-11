// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayTagContainer.h"
#include "BTDecorator_HasTag.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UBTDecorator_HasTag : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasTag();

protected:
	// ���� �˻縦 �����ϴ� ���� �Լ�
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	// �����Ϳ��� �˻��� �±׸� ������ ����
	UPROPERTY(EditAnywhere, Category = "TagCheck")
	FGameplayTag TagToCheck;
};
