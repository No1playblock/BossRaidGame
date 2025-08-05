// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_TargetToFind.h"
#include "AI/BRAIController.h"
#include "Character/NonPlayerGASCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/OverlapResult.h"
#include "Character/MobGASCharacter.h"

UBTService_TargetToFind::UBTService_TargetToFind()
{
	NodeName = "Target Finder";
	bNotifyBecomeRelevant = true;
	// 서비스가 각 AI 인스턴스별로 노드 메모리를 생성하도록 설정
	bCreateNodeInstance = true;
}
void UBTService_TargetToFind::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	CachedController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (CachedController)
	{
		CachedMobCharacter = Cast<ANonPlayerGASCharacter>(CachedController->GetPawn());
		CachedBlackboard = CachedController->GetBlackboardComponent();
		UE_LOG(LogTemp, Warning, TEXT("BTService_TargetToFind OnBecomeRelevant called. Controller: %s, Character: %s"), 
			*CachedController->GetName(), *CachedMobCharacter->GetName());
	}
}

void UBTService_TargetToFind::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!CachedController || !CachedMobCharacter || !CachedBlackboard)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, CachedMobCharacter);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		CachedMobCharacter->GetActorLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel2, 
		FCollisionShape::MakeSphere(Radius),
		CollisionQueryParam
	);

	if (bResult)
	{
		AActor* FoundTarget = nullptr;
		for (const auto& Overlap : OverlapResults)
		{
			
			if (Overlap.GetActor() && Overlap.GetActor() != CachedMobCharacter)
			{
				FoundTarget = Overlap.GetActor();
				break;
			}
		}

		if (FoundTarget)
		{
			const float Distance = CachedMobCharacter->GetDistanceTo(FoundTarget);
			
			CachedBlackboard->SetValueAsObject(TEXT("Target"), FoundTarget);
			CachedBlackboard->SetValueAsFloat(TEXT("Distance"), Distance);
			CachedBlackboard->SetValueAsVector(TEXT("TargetLocation"), FoundTarget->GetActorLocation());
			
			if(AMobGASCharacter* MobCharacter = Cast<AMobGASCharacter>(CachedMobCharacter))
			{
				MobCharacter->SetChasingState();
			}
		
		}
	}
	else
	{
		CachedBlackboard->ClearValue(TEXT("Target")); 
		CachedBlackboard->ClearValue(TEXT("Distance"));
		CachedBlackboard->ClearValue(TEXT("TargetLocation"));

		if (AMobGASCharacter* MobCharacter = Cast<AMobGASCharacter>(CachedMobCharacter))
		{
			MobCharacter->SetWalkingState();
		}
	}
}

