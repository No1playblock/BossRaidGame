// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/AT_SphereTraceForTargets.h"
#include "Kismet/KismetSystemLibrary.h"
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
	/*UE_LOG(LogTemp, Warning, TEXT("SphereTraceForTargets: Channel=%d")
		, static_cast<int32>(TargetChannel));*/

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(TargetChannel)); // Pawn 채널과 충돌하는지 확인
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		SpherePos,        // 구의 중심 위치
		SphereRadius,     // 구의 반지름
		ObjectTypes,      // 검사할 오브젝트 타입들 (Pawn)
		nullptr,          // 특정 클래스 필터
		ActorsToIgnore,
		HitResults         // 결과가 저장될 액터 배열
	);

	const FColor DebugColor = HitResults.Num() > 0 ? FColor::Green : FColor::Red;
	DrawDebugSphere(
		GetWorld(),
		SpherePos,      // 구의 중심 위치
		SphereRadius,   // 구의 반지름
		24,             // 구의 부드러움 (Segment)
		DebugColor,     // 색상
		false,          // 계속 유지할지 여부
		0.2f            // 표시될 시간 (초)
	);

	OnTargetsFound.Broadcast(HitResults);

	EndTask();
}
