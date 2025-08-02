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
		// 올바른 시그니처를 가진 함수를 델리게이트에 바인딩
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
				// 델리게이트가 남아있지 않도록 정리
				ASC->OnAbilityEnded.RemoveAll(this);

				// 취소할 태그를 담을 컨테이너를 지역 변수로 생성
				FGameplayTagContainer TagsToCancel;
				TagsToCancel.AddTag(AbilityTag);

				//생성된 지역 변수의 주소를 전달하여 어빌리티를 취소
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

	// Owner로부터 AbilitySystemComponent를 다시 가져옴
	AAIController* AIController = Cast<AAIController>(CachedOwnerComp->GetOwner());
	if (!AIController) return;
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(AIController->GetPawn());
	if (!AbilitySystemInterface) return;
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC) return;

	// EndedData에서 받은 핸들(Handle)을 사용하여 어빌리티 스펙(Spec)을 찾음.
	const FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(EndedData.AbilitySpecHandle);

	// 스펙에서 실제 어빌리티(Ability) 객체를 가져옵니다.
	const UGameplayAbility* EndedAbility = AbilitySpec ? AbilitySpec->Ability : nullptr;

	//종료된 어빌리티가 우리가 실행한 어빌리티가 맞는지 태그로 확인
	if (EndedAbility && EndedAbility->AbilityTags.HasTag(AbilityTag))
	{
		// 델리게이트를 반드시 정리
		ASC->OnAbilityEnded.RemoveAll(this);

		// 비헤이비어 트리에 태스크가 '성공'으로 끝났다고 알림
		FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}
