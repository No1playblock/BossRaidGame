// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"

UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector StartLocation, FVector EndLocation, float Radius, bool bDebug)
{
	// ���ο� �½�ũ �ν��Ͻ� ����
	UAT_SphereTraceForTargets* NewTask = NewAbilityTask<UAT_SphereTraceForTargets>(OwningAbility);

	// ���丮 �Լ��� ���� ���� �Ķ���͵��� �½�ũ�� ��� ������ ����
	NewTask->TraceStart = StartLocation;
	NewTask->TraceEnd = EndLocation;
	NewTask->TraceRadius = Radius;
	NewTask->bShowDebug = bDebug;

	return NewTask;
}

void UAT_SphereTraceForTargets::Activate()
{
	// SphereTraceMulti�� ����Ͽ� �浹 �˻� ����
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActor()); // �ڽ��� ĳ���ʹ� �˻翡�� ����

	TArray<FHitResult> HitResults;

	UKismetSystemLibrary::SphereTraceMulti(
		this, // World Context Object
		TraceStart,
		TraceEnd,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn), // Pawn ä�ΰ� �浹�ϴ��� Ȯ��
		false,
		ActorsToIgnore,
		bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true
	);

	// ����� ������ OnTargetsFound ��������Ʈ�� ���� ����� ��ε�ĳ��Ʈ
	OnTargetsFound.Broadcast(HitResults);

	// �½�ũ�� ��� �ӹ��� �������Ƿ� ����
	EndTask();
}
