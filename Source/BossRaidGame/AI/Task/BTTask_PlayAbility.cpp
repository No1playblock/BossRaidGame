// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PlayAbility.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_PlayAbility::UBTTask_PlayAbility()
{
	NodeName = "Play Ability By Tag";
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTTask_PlayAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)))
	{
		// �ùٸ� �ñ״�ó�� ���� �Լ��� ��������Ʈ�� ���ε�
		ASC->OnAbilityEnded.AddUObject(this, &UBTTask_PlayAbility::OnAbilityEnded);
		CachedOwnerComp = &OwnerComp;
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_PlayAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
		if (AbilitySystemInterface)
		{
			UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
			if (ASC)
			{
				// ��������Ʈ�� �������� �ʵ��� ����
				ASC->OnAbilityEnded.RemoveAll(this);

				// ����� �±׸� ���� �����̳ʸ� ���� ������ ����
				FGameplayTagContainer TagsToCancel;
				TagsToCancel.AddTag(AbilityTag);

				//������ ���� ������ �ּҸ� �����Ͽ� �����Ƽ�� ���
				ASC->CancelAbilities(&TagsToCancel);
			}
		}
	}

	return EBTNodeResult::Aborted;
}

void UBTTask_PlayAbility::OnAbilityEnded(const FAbilityEndedData& EndedData)
{
	if (!CachedOwnerComp.IsValid())
	{
		return;
	}

	// Owner�κ��� AbilitySystemComponent�� �ٽ� ������
	AAIController* AIController = Cast<AAIController>(CachedOwnerComp->GetOwner());
	if (!AIController) return;
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return;
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC) return;

	// EndedData���� ���� �ڵ�(Handle)�� ����Ͽ� �����Ƽ ����(Spec)�� ã��.
	const FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(EndedData.AbilitySpecHandle);

	// ���忡�� ���� �����Ƽ(Ability) ��ü�� ������
	const UGameplayAbility* EndedAbility = AbilitySpec ? AbilitySpec->Ability : nullptr;

	if (EndedAbility && EndedAbility->AbilityTags.HasTag(AbilityTag))
	{
		// ��������Ʈ�� �ݵ�� ����
		ASC->OnAbilityEnded.RemoveAll(this);

		// �����̺�� Ʈ���� �½�ũ�� '����'���� �����ٰ� �˸�
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}
