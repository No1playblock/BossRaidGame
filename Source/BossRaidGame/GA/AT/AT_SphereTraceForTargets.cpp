// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
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
	/*UE_LOG(LogTemp, Warning, TEXT("SphereTraceForTargets: Channel=%d")
		, static_cast<int32>(TargetChannel));*/

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(TargetChannel)); // Pawn ä�ΰ� �浹�ϴ��� Ȯ��
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		SpherePos,        // ���� �߽� ��ġ
		SphereRadius,     // ���� ������
		ObjectTypes,      // �˻��� ������Ʈ Ÿ�Ե� (Pawn)
		nullptr,          // Ư�� Ŭ���� ����
		ActorsToIgnore,
		HitResults         // ����� ����� ���� �迭
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

	OnTargetsFound.Broadcast(HitResults);

	EndTask();
}
