// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"

UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel)
{
	UAT_SphereTraceForTargets* NewTask = NewAbilityTask<UAT_SphereTraceForTargets>(OwningAbility);

	// 팩토리 함수를 통해 받은 파라미터들을 태스크의 멤버 변수에 저장
	NewTask->SpherePos = TraceSpherePos;
	NewTask->SphereRadius = Radius;
	NewTask->TargetChannel = Channel;
	return NewTask;
}
void UAT_SphereTraceForTargets::Activate()
{
	// SphereTraceMulti를 사용하여 충돌 검사 수행
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActor()); // 자신의 캐릭터는 검사에서 제외

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

	//const FColor DebugColor = HitResults.Num() > 0 ? FColor::Green : FColor::Red;
	//DrawDebugSphere(
	//	GetWorld(),
	//	SpherePos,      // 구의 중심 위치
	//	SphereRadius,   // 구의 반지름
	//	24,             // 구의 부드러움 (Segment)
	//	DebugColor,     // 색상
	//	false,          // 계속 유지할지 여부
	//	0.2f            // 표시될 시간 (초)
	//);

	OnTargetsFound.Broadcast(OverlapResults);

	EndTask();
}
