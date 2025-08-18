// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"

UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel)
{
	UAT_SphereTraceForTargets* NewTask = NewAbilityTask<UAT_SphereTraceForTargets>(OwningAbility);

	// ���丮 �Լ��� ���� ���� �Ķ���͵��� �½�ũ�� ��� ������ ����
	NewTask->SpherePos = TraceSpherePos;
	NewTask->SphereRadius = Radius;
	NewTask->TargetChannel = Channel;
	return NewTask;
}
void UAT_SphereTraceForTargets::Activate()
{
	// SphereTraceMulti�� ����Ͽ� �浹 �˻� ����
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActor()); // �ڽ��� ĳ���ʹ� �˻翡�� ����

	TArray<AActor*> HitResults;
	TArray<FOverlapResult> OverlapResults;

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		SpherePos,
		FQuat::Identity,
		TargetChannel, 
		SphereShape,
		QueryParams
	);

	const FColor DebugColor = HitResults.Num() > 0 ? FColor::Green : FColor::Red;
	DrawDebugSphere(
		GetWorld(),
		SpherePos,      // ���� �߽� ��ġ
		SphereRadius,   // ���� ������
		24,             // ���� �ε巯�� (Segment)
		DebugColor,     // ����
		false,          // ��� �������� ����
		0.2f            // ǥ�õ� �ð� (��)
	);

	OnTargetsFound.Broadcast(OverlapResults);

	EndTask();
}
