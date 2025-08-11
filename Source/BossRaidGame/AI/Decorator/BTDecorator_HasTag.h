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
	// 조건 검사를 수행하는 메인 함수
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	// 에디터에서 검사할 태그를 설정할 변수
	UPROPERTY(EditAnywhere, Category = "TagCheck")
	FGameplayTag TagToCheck;
};
