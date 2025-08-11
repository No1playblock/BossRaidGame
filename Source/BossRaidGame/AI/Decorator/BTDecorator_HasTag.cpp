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
	// �θ� Ŭ������ �Լ��� ���� ȣ���ϴ� ���� ����
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	// ��Ʈ�ѷ��� �����ϴ� ���� IAbilitySystemInterface�� �����ߴ��� Ȯ��
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface)
	{
		return false;
	}

	// �����Ƽ �ý��� ������Ʈ(ASC)�� ������
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	// ASC�� �츮�� ������ �±׸� ������ �ִ��� Ȯ���Ͽ� ����� ��ȯ
	return ASC->HasMatchingGameplayTag(TagToCheck);
}
