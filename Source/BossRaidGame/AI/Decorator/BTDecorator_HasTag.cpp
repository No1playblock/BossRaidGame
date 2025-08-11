// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_HasTag.h"
#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UBTDecorator_HasTag::UBTDecorator_HasTag()
{
	NodeName = "Has Gameplay Tag";
}

bool UBTDecorator_HasTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// 부모 클래스의 함수를 먼저 호출하는 것이 안전
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	// 컨트롤러가 조종하는 폰이 IAbilitySystemInterface를 구현했는지 확인
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface)
	{
		return false;
	}

	// 어빌리티 시스템 컴포넌트(ASC)를 가져옴
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	// ASC가 우리가 지정한 태그를 가지고 있는지 확인하여 결과를 반환
	return ASC->HasMatchingGameplayTag(TagToCheck);
}
