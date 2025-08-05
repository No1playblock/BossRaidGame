// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "Character/NonPlayerGASCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");

	// Details 패널에서 설정할 블랙보드 키가 'Name' 타입이 되도록 필터링
	BlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ExecuteAttack, BlackboardKey));
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	ANonPlayerGASCharacter* Character = AIController ? Cast<ANonPlayerGASCharacter>(AIController->GetPawn()) : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!Character || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 블랙보드에서 'Name' 타입으로 키 값을 읽어옴
	const FName AttackTagName = BlackboardComp->GetValueAsName(GetSelectedBlackboardKey());

	if (AttackTagName.IsNone())
	{
		return EBTNodeResult::Failed;
	}

	AttackAbilityTag = FGameplayTag::RequestGameplayTag(AttackTagName);
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (ASC && AttackAbilityTag.IsValid())
	{
		AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ExecuteAttack::OnAbilityEnded);

		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(AttackAbilityTag);
		if (ASC->TryActivateAbilitiesByTag(TagContainer))
		{
			CachedOwnerComp = &OwnerComp;
			return EBTNodeResult::InProgress;
		}
	}

	if (ASC)
	{
		ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	return EBTNodeResult::Failed;
}
void UBTTask_ExecuteAttack::OnAbilityEnded(const FAbilityEndedData& EndedData)
{
	if (EndedData.AbilityThatEnded && EndedData.AbilityThatEnded->AbilityTags.HasTag(AttackAbilityTag))
	{
		if (UBehaviorTreeComponent* OwnerComp = CachedOwnerComp.Get())
		{
			if (UAbilitySystemComponent* ASC = OwnerComp->GetAIOwner()->GetPawn()->FindComponentByClass<UAbilitySystemComponent>())
			{
				ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
			}

			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

EBTNodeResult::Type UBTTask_ExecuteAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UAbilitySystemComponent* ASC = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UAbilitySystemComponent>())
	{
		ASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	return EBTNodeResult::Aborted;
}