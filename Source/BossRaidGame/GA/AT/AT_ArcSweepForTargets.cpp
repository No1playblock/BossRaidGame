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

	// 변수 초기화 및 첫 번째 트레이스 시작
	CurrentSphereIndex = 0;
	CombinedOverlapResults.Empty();
	UniqueHitActors.Empty();
	PerformNextSphereTrace();
}

void UAT_ArcSweepForTargets::PerformNextSphereTrace()
{
	// 모든 구체를 다 그렸으면 태스크 종료
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

	// 액터의 위치와 회전값을 기준으로 호의 '중심점'을 계산
	const FVector ActorLocation = OwnerActor->GetActorLocation();
	const FRotator ActorRotation = OwnerActor->GetActorRotation();
	const FVector ArcCenter = ActorLocation + ActorRotation.RotateVector(ArcCenterOffset);

	// 액터의 현재 방향에 ArcStartYaw를 더해 호의 '시작 방향'을 계산
	const FRotator ArcStartRotation = ActorRotation + FRotator(0.f, ArcStartYaw, 0.f);
	const FVector StartDirection = ArcStartRotation.Vector();

	const float AngleStepRad = SphereRadius / ArcRadius;
	const FVector RotationAxis = FVector::UpVector; // 수평으로만 회전

	// 현재 인덱스에 해당하는 구체 하나만 계산
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
		//DrawDebugSphere(World, SphereCenter, SphereRadius, 24, SphereOverlapResults.Num() > 0 ? FColor::Green : FColor::Red, false, DebugLifetime);
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

	// 다음 인덱스로 넘어가고, 다음 함수 호출을 딜레이만큼 예약
	CurrentSphereIndex++;
	if (DelayBetweenSpheres > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAT_ArcSweepForTargets::PerformNextSphereTrace, DelayBetweenSpheres, false);
	}
	else
	{
		// 딜레이가 0이면 바로 다음 루프 실행
		PerformNextSphereTrace();
	}
}

void UAT_ArcSweepForTargets::OnDestroy(bool bInOwnerFinished)
{
	// 태스크가 중간에 취소될 경우 타이머가 계속 실행되는 것을 방지
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	Super::OnDestroy(bInOwnerFinished);
}