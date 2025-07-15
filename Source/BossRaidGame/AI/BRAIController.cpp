// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/NonPlayerGASCharacter.h"

ABRAIController::ABRAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	check(BlackboardComp);
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
	check(BehaviorTreeComp);
}

void ABRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ANonPlayerGASCharacter* possessedPawn = Cast<ANonPlayerGASCharacter>(InPawn);

	if (InPawn != nullptr)
	{
		BlackboardComp->InitializeBlackboard(*(possessedPawn->GetBehaviorTree()->BlackboardAsset));

		BehaviorTreeComp->StartTree(*(possessedPawn->GetBehaviorTree()));
		GetBlackboardComp()->SetValueAsVector(TEXT("HomeLocation"), InPawn->GetActorLocation());
	}
}
