// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_ChooseAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/BossGASCharacter.h"
#include "AIController.h"

UBTTask_ChooseAttack::UBTTask_ChooseAttack()
{
	// 에디터에서 보여질 노드의 이름 설정
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

	// 선택 정책에 따라 공격 태그 목록을 캐릭터로부터 가져옴
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
		// 목록에서 랜덤한 태그를 하나 선택
		const FGameplayTag ChosenAttackTag = AttackPool[FMath::RandRange(0, AttackPool.Num() - 1)];

		// 블랙보드에 선택된 '태그'를 저장
		BlackboardComp->SetValueAsName(SelectedAttackTagKey.SelectedKeyName, ChosenAttackTag.GetTagName());
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
