// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_SetRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BRAIController.h"
#include "Character/NonPlayerGASCharacter.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

EBTNodeResult::Type UBTTask_SetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* ThisAIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!ThisAIController) 	return EBTNodeResult::Failed;

	ANonPlayerGASCharacter* ThisMobCharacter = Cast<ANonPlayerGASCharacter>(ThisAIController->GetPawn());
	if (!ThisMobCharacter) 	return EBTNodeResult::Failed;

	UNavigationSystemV1* Navsystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (Navsystem == nullptr) return EBTNodeResult::Failed;

	FNavLocation RandomNavLocation;
	bool isRandomPoint = Navsystem->GetRandomPointInNavigableRadius(ThisMobCharacter->GetActorLocation(), Radius, RandomNavLocation, NULL);

	if (isRandomPoint)
	{
		//DrawDebugPoint(
		//	GetWorld(),                 // ���� ����
		//	RandomNavLocation.Location, // ���� �׸� ��ġ (FNavLocation���� FVector�� ������)
		//	20.0f,                      // ���� ũ�� (������)
		//	FColor::Green,              // ���� ����
		//	false,                      // �� �����Ӹ� ������ ���� (false = ����)
		//	5.0f                        // �� �� ���� ������ (���� �ð�)
		//);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("RandomLocation"), RandomNavLocation.Location);
		
		return EBTNodeResult::Succeeded;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to find a random navigable point within radius: %f"), Radius);
	return EBTNodeResult::Failed;
}
