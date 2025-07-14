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

UBTService_TargetToFind::UBTService_TargetToFind()
{
	NodeName = "Target Finder";
	bNotifyBecomeRelevant = true;
	// 서비스가 각 AI 인스턴스별로 노드 메모리를 생성하도록 설정합니다.
	bCreateNodeInstance = true;
}
void UBTService_TargetToFind::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	CachedController = Cast<ABRAIController>(OwnerComp.GetAIOwner());
	if (CachedController)
	{
		CachedMobCharacter = Cast<ANonPlayerGASCharacter>(CachedController->GetPawn());
		CachedBlackboard = CachedController->GetBlackboardComp();
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
			CachedBlackboard->SetValueAsObject(TEXT("Target"), FoundTarget);
			DrawDebugSphere(GetWorld(), CachedMobCharacter->GetActorLocation(), Radius, 12, FColor::Green, false, Interval);
			CachedMobCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		}
	}
	else
	{
		CachedBlackboard->ClearValue(TEXT("Target")); 
		DrawDebugSphere(GetWorld(), CachedMobCharacter->GetActorLocation(), Radius, 12, FColor::Red, false, Interval);
		CachedMobCharacter->GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

