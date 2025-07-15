// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"

//UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector StartLocation, FVector EndLocation, float Radius, bool bDebug, ECollisionChannel Channel)
//{
//	// ���ο� �½�ũ �ν��Ͻ� ����
//	UAT_SphereTraceForTargets* NewTask = NewAbilityTask<UAT_SphereTraceForTargets>(OwningAbility);
//
//	// ���丮 �Լ��� ���� ���� �Ķ���͵��� �½�ũ�� ��� ������ ����
//	NewTask->TraceStart = StartLocation;
//	NewTask->TraceEnd = EndLocation;
//	NewTask->TraceRadius = Radius;
//	NewTask->bShowDebug = bDebug;
//	NewTask->TargetChannel = Channel;
//	return NewTask;
//}

UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel)
{
	// ���ο� �½�ũ �ν��Ͻ� ����
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
	UE_LOG(LogTemp, Warning, TEXT("SphereTraceForTargets: Channel=%d")
		, static_cast<int32>(TargetChannel));
	//UKismetSystemLibrary::SphereTraceMulti(
	//	this, // World Context Object
	//	TraceStart,
	//	TraceEnd,
	//	TraceRadius,
	//	UEngineTypes::ConvertToTraceType(TargetChannel), // Pawn ä�ΰ� �浹�ϴ��� Ȯ��
	//	false,
	//	ActorsToIgnore,
	//	bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	//	HitResults,
	//	true
	//);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(TargetChannel)); // Pawn ä�ΰ� �浹�ϴ��� Ȯ��
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		SpherePos,        // ���� �߽� ��ġ
		SphereRadius,     // ���� ������
		ObjectTypes,      // �˻��� ������Ʈ Ÿ�Ե� (Pawn)
		nullptr,          // Ư�� Ŭ���� ���� (��� �� ��)
		ActorsToIgnore,
		HitResults         // ����� ����� ���� �迭
	);

	// ����� ������ OnTargetsFound ��������Ʈ�� ���� ����� ��ε�ĳ��Ʈ
	OnTargetsFound.Broadcast(HitResults);

	// �½�ũ�� ��� �ӹ��� �������Ƿ� ����
	EndTask();
}
