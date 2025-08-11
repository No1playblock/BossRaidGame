// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_SmoothFaceTarget.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_SmoothFaceTarget::UBTTask_SmoothFaceTarget()
{
	NodeName = TEXT("Smooth Face Target");

	// �� �½�ũ�� TickTask �Լ��� ������� �����̺�� Ʈ���� �˸��ϴ�.
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SmoothFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Ÿ���� ������ �ٷ� ���� ó��
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp || BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()) == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// �½�ũ�� ���� ���� ������ �˸���, TickTask�� ȣ��ǵ��� ��
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
		// �߰��� ���̳� Ÿ���� ������� �½�ũ ����
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// ��ǥ ȸ����(Yaw) ���
	FVector DirectionToTarget = TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation();
	DirectionToTarget.Z = 0; // �������θ� ȸ���ϵ��� Z���� ����
	FRotator TargetRotation = DirectionToTarget.Rotation();

	//���� ȸ�������� ��ǥ ȸ�������� �ε巴�� ����
	FRotator CurrentRotation = OwnerPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);

	//���� �� ȸ������ ���� ���� (Yaw�� �����Ͽ� ���� ȸ�� ����)
	OwnerPawn->SetActorRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));

	//��ǥ ������ ����� ����������� Ȯ��
	if (FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw)) < Precision)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
