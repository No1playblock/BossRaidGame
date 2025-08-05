// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_ChooseAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BossGASCharacter.h"
#include "AIController.h"

UBTTask_ChooseAttack::UBTTask_ChooseAttack()
{
	// �����Ϳ��� ������ ����� �̸� ����
	NodeName = TEXT("Choose Attack");
}

EBTNodeResult::Type UBTTask_ChooseAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ABossGASCharacter* Character = Cast<ABossGASCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!Character || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// ���� ��å�� ���� ���� �±� ����� ĳ���ͷκ��� ������
	TArray<FGameplayTag> AttackPool;
	switch (AttackPolicy)
	{
	case EAttackSelectionPolicy::MeleeOnly:
		AttackPool = Character->GetMeleeAttackTags();
		break;
	case EAttackSelectionPolicy::RangedOnly:
		AttackPool = Character->GetRangedAttackTags();
		break;
	case EAttackSelectionPolicy::All:
		AttackPool.Append(Character->GetMeleeAttackTags());
		AttackPool.Append(Character->GetRangedAttackTags());
		break;
	}

	if (AttackPool.Num() > 0)
	{
		// ��Ͽ��� ������ �±׸� �ϳ� ����
		const FGameplayTag ChosenAttackTag = AttackPool[FMath::RandRange(0, AttackPool.Num() - 1)];

		// �����忡 ���õ� '�±�'�� ����
		BlackboardComp->SetValueAsName(SelectedAttackTagKey.SelectedKeyName, ChosenAttackTag.GetTagName());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
