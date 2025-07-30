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
		DrawDebugPoint(
			GetWorld(),                 // 현재 월드
			RandomNavLocation.Location, // 점을 그릴 위치 (FNavLocation에서 FVector를 가져옴)
			20.0f,                      // 점의 크기 (반지름)
			FColor::Green,              // 점의 색상
			false,                      // 한 프레임만 보일지 여부 (false = 지속)
			5.0f                        // 몇 초 동안 보일지 (지속 시간)
		);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("RandomLocation"), RandomNavLocation.Location);
		/*UAIBlueprintHelperLibrary::SimpleMoveToLocation(ThisAIController, RandomNavLocation.Location);
		UE_LOG(LogTemp, Warning, TEXT("Moving to random location: %s"), *RandomNavLocation.Location.ToString());*/
		return EBTNodeResult::Succeeded;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to find a random navigable point within radius: %f"), Radius);
	return EBTNodeResult::Failed;
}
