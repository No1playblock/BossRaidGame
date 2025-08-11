// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_SmoothFaceTarget.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_SmoothFaceTarget::UBTTask_SmoothFaceTarget()
{
	NodeName = TEXT("Smooth Face Target");

	// 이 태스크가 TickTask 함수를 사용함을 비헤이비어 트리에 알립니다.
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SmoothFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 타겟이 없으면 바로 실패 처리
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp || BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()) == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 태스크가 아직 진행 중임을 알리고, TickTask가 호출되도록 함
	return EBTNodeResult::InProgress;
}

void UBTTask_SmoothFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* OwnerPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));

	if (!OwnerPawn || !TargetActor)
	{
		// 중간에 폰이나 타겟이 사라지면 태스크 실패
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 목표 회전값(Yaw) 계산
	FVector DirectionToTarget = TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation();
	DirectionToTarget.Z = 0; // 수평으로만 회전하도록 Z축은 무시
	FRotator TargetRotation = DirectionToTarget.Rotation();

	//현재 회전값에서 목표 회전값으로 부드럽게 보간
	FRotator CurrentRotation = OwnerPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);

	//계산된 새 회전값을 폰에 적용 (Yaw만 적용하여 수평 회전 보장)
	OwnerPawn->SetActorRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));

	//목표 각도에 충분히 가까워졌는지 확인
	if (FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw)) < Precision)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
