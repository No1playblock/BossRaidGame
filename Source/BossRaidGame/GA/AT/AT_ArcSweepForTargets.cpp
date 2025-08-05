// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_ArcSweepForTargets.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UAT_ArcSweepForTargets* UAT_ArcSweepForTargets::ArcSweepForTargets(UGameplayAbility* OwningAbility, FVector InArcCenterOffset, float InArcRadius, float InSphereRadius, int32 InSphereNum, float InArcStartYaw, bool bInIsClockwise, float InDelayBetweenSpheres, ECollisionChannel InTraceChannel, bool bInDrawDebug)
{
	UAT_ArcSweepForTargets* MyTask = NewAbilityTask<UAT_ArcSweepForTargets>(OwningAbility);
	MyTask->ArcCenterOffset = InArcCenterOffset;
	MyTask->ArcRadius = InArcRadius;
	MyTask->SphereRadius = InSphereRadius;
	MyTask->SphereNum = InSphereNum;
	MyTask->ArcStartYaw = InArcStartYaw;
	MyTask->bIsClockwise = bInIsClockwise;
	MyTask->DelayBetweenSpheres = InDelayBetweenSpheres;
	MyTask->TraceChannel = InTraceChannel;
	MyTask->bDrawDebug = bInDrawDebug;
	return MyTask;
}

void UAT_ArcSweepForTargets::Activate()
{
	Super::Activate();

	// ���� �ʱ�ȭ �� ù ��° Ʈ���̽� ����
	CurrentSphereIndex = 0;
	CombinedOverlapResults.Empty();
	UniqueHitActors.Empty();
	PerformNextSphereTrace();
}

void UAT_ArcSweepForTargets::PerformNextSphereTrace()
{
	// ��� ��ü�� �� �׷����� �½�ũ ����
	if (CurrentSphereIndex >= SphereNum)
	{
		if (CombinedOverlapResults.Num() > 0)
		{
			OnTargetsFound.Broadcast(CombinedOverlapResults);
		}
		EndTask();
		return;
	}

	AActor* OwnerActor = GetAvatarActor();
	if (!OwnerActor)
	{
		EndTask();
		return;
	}

	UWorld* World = GetWorld();

	// ������ ��ġ�� ȸ������ �������� ȣ�� '�߽���'�� ���
	const FVector ActorLocation = OwnerActor->GetActorLocation();
	const FRotator ActorRotation = OwnerActor->GetActorRotation();
	const FVector ArcCenter = ActorLocation + ActorRotation.RotateVector(ArcCenterOffset);

	// ������ ���� ���⿡ ArcStartYaw�� ���� ȣ�� '���� ����'�� ���
	const FRotator ArcStartRotation = ActorRotation + FRotator(0.f, ArcStartYaw, 0.f);
	const FVector StartDirection = ArcStartRotation.Vector();

	const float AngleStepRad = SphereRadius / ArcRadius;
	const FVector RotationAxis = FVector::UpVector; // �������θ� ȸ��

	// ���� �ε����� �ش��ϴ� ��ü �ϳ��� ���
	const float CurrentAngleRad = CurrentSphereIndex * AngleStepRad * (bIsClockwise ? -1.f : 1.f);
	const float CurrentAngleDeg = FMath::RadiansToDegrees(CurrentAngleRad);
	const FVector CurrentDirection = StartDirection.RotateAngleAxis(CurrentAngleDeg, RotationAxis);
	const FVector SphereCenter = ArcCenter + CurrentDirection * ArcRadius;

	TArray<FOverlapResult> SphereOverlapResults;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
	World->OverlapMultiByChannel(SphereOverlapResults, SphereCenter, FQuat::Identity, TraceChannel, SphereShape);

	if (bDrawDebug)
	{
		float DebugLifetime = DelayBetweenSpheres > 0.f ? DelayBetweenSpheres : 0.5f;
		DrawDebugSphere(World, SphereCenter, SphereRadius, 24, SphereOverlapResults.Num() > 0 ? FColor::Green : FColor::Red, false, DebugLifetime);
	}

	for (const FOverlapResult& Overlap : SphereOverlapResults)
	{
		if (Overlap.GetActor() && Overlap.GetActor() != OwnerActor && !UniqueHitActors.Contains(Overlap.GetActor()))
		{
			CombinedOverlapResults.Add(Overlap);
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Overlap.GetActor()->GetName());
			UniqueHitActors.Add(Overlap.GetActor());
		}
	}

	// ���� �ε����� �Ѿ��, ���� �Լ� ȣ���� �����̸�ŭ ����
	CurrentSphereIndex++;
	if (DelayBetweenSpheres > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAT_ArcSweepForTargets::PerformNextSphereTrace, DelayBetweenSpheres, false);
	}
	else
	{
		// �����̰� 0�̸� �ٷ� ���� ���� ����
		PerformNextSphereTrace();
	}
}

void UAT_ArcSweepForTargets::OnDestroy(bool bInOwnerFinished)
{
	// �½�ũ�� �߰��� ��ҵ� ��� Ÿ�̸Ӱ� ��� ����Ǵ� ���� ����
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Super::OnDestroy(bInOwnerFinished);
}