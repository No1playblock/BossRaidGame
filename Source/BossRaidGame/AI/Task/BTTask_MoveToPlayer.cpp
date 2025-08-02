// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_MoveToPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MoveToPlayer::UBTTask_MoveToPlayer()
{
    NodeName = TEXT("MoveToPlayer");

    TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToPlayer, TargetKey), AActor::StaticClass());

    bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!TargetActor) return EBTNodeResult::Failed;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

    FNavPathSharedPtr NavPath;
    FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest, &NavPath);

    if (MoveResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        return EBTNodeResult::Succeeded;
    }

    if (MoveResult.Code == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    MoveRequestID = MoveResult.MoveId;
    CachedOwnerComp = &OwnerComp;

    // 이동 완료 이벤트 바인딩
    AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
        this, &UBTTask_MoveToPlayer::OnMoveCompleted
    );

    return EBTNodeResult::InProgress;

}
EBTNodeResult::Type UBTTask_MoveToPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController && AIController->GetPathFollowingComponent())
    {
        // 이동을 멈추고 델리게이트를 정리
        AIController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
        AIController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
    }
    return EBTNodeResult::Aborted;
}


void UBTTask_MoveToPlayer::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (RequestID != MoveRequestID || !CachedOwnerComp) return;

    AAIController* AIController = CachedOwnerComp->GetAIOwner();
    if (AIController)
    {
        AIController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
    }

    EBTNodeResult::Type FinalResult = Result.IsSuccess()
        ? EBTNodeResult::Succeeded
        : EBTNodeResult::Failed;

    FinishLatentTask(*CachedOwnerComp, FinalResult);
}
