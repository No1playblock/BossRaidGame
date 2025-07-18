// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"

//UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector StartLocation, FVector EndLocation, float Radius, bool bDebug, ECollisionChannel Channel)
//{
//	// 새로운 태스크 인스턴스 생성
//	UAT_SphereTraceForTargets* NewTask = NewAbilityTask<UAT_SphereTraceForTargets>(OwningAbility);
//
//	// 팩토리 함수를 통해 받은 파라미터들을 태스크의 멤버 변수에 저장
//	NewTask->TraceStart = StartLocation;
//	NewTask->TraceEnd = EndLocation;
//	NewTask->TraceRadius = Radius;
//	NewTask->bShowDebug = bDebug;
//	NewTask->TargetChannel = Channel;
//	return NewTask;
//}

UAT_SphereTraceForTargets* UAT_SphereTraceForTargets::SphereTraceForTargets(UGameplayAbility* OwningAbility, FVector TraceSpherePos, float Radius, ECollisionChannel Channel)
{
	// 새로운 태스크 인스턴스 생성
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
	UE_LOG(LogTemp, Warning, TEXT("SphereTraceForTargets: Channel=%d")
		, static_cast<int32>(TargetChannel));
	//UKismetSystemLibrary::SphereTraceMulti(
	//	this, // World Context Object
	//	TraceStart,
	//	TraceEnd,
	//	TraceRadius,
	//	UEngineTypes::ConvertToTraceType(TargetChannel), // Pawn 채널과 충돌하는지 확인
	//	false,
	//	ActorsToIgnore,
	//	bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	//	HitResults,
	//	true
	//);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(TargetChannel)); // Pawn 채널과 충돌하는지 확인
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		SpherePos,        // 구의 중심 위치
		SphereRadius,     // 구의 반지름
		ObjectTypes,      // 검사할 오브젝트 타입들 (Pawn)
		nullptr,          // 특정 클래스 필터 (사용 안 함)
		ActorsToIgnore,
		HitResults         // 결과가 저장될 액터 배열
	);

	// 결과가 있으면 OnTargetsFound 델리게이트를 통해 결과를 브로드캐스트
	OnTargetsFound.Broadcast(HitResults);

	// 태스크의 모든 임무가 끝났으므로 종료
	EndTask();
}
