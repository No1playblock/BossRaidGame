// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/NonPlayerGASCharacter.h"
#include "Navigation/CrowdFollowingComponent.h"


ABRAIController::ABRAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	check(BlackboardComp);
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	check(BehaviorTreeComp);
}

void ABRAIController::BeginPlay()
{
	Super::BeginPlay();


	UCrowdFollowingComponent* CrowdFollowingComp = FindComponentByClass<UCrowdFollowingComponent>();

	if (CrowdFollowingComp)
	{
		/*CrowdFollowingComp->SetCrowdSeparation(true);
		CrowdFollowingComp->SetCrowdSeparationWeight(SeperationWeight);
		CrowdFollowingComp->SetCrowdAvoidanceRangeMultiplier(AvoidanceRangeMultiplier);
		CrowdFollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
		CrowdFollowingComp->SetCrowdPathOffset(true);*/

	}
}

void ABRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ANonPlayerGASCharacter* possessedPawn = Cast<ANonPlayerGASCharacter>(InPawn);

	if (InPawn != nullptr)
	{
		if (BlackboardComp && BehaviorTreeComp)
		{
			BlackboardComp->InitializeBlackboard(*(possessedPawn->GetBehaviorTree()->BlackboardAsset));

			BehaviorTreeComp->StartTree(*(possessedPawn->GetBehaviorTree()));
			GetBlackboardComp()->SetValueAsVector(TEXT("HomeLocation"), InPawn->GetActorLocation());
		}
		
	}
}
