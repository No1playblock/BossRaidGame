#include "AI/Service/BTService_MTTargetToFind.h"
#include "AIController.h"
#include "Character/NonPlayerGASCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/MobGASCharacter.h"
#include "Engine/OverlapResult.h"
#include "Async/AsyncWork.h"
#include "Stats/Stats.h"
#include "Character/GASCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
class FAsyncTargetFindTask
{
public:
	FAsyncTargetFindTask(UWorld* InWorld, FVector InOrigin, float InRadius, AActor* InSelfActor, ECollisionChannel InTraceChannel, TFunction<void(bool)> InCallback)
		: World(InWorld), Origin(InOrigin), Radius(InRadius), SelfActor(InSelfActor), TraceChannel(InTraceChannel), OnDoneCallback(MoveTemp(InCallback))
	{
	}

	void DoWork()
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(AsyncTargetFind_DoWork_SimpleCheck);

		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, SelfActor);

		// 겹치는 것이 있는지 '여부'만 빠르게 확인
		bool bFoundSomething = World->OverlapMultiByObjectType(
			OverlapResults, Origin, FQuat::Identity, FCollisionObjectQueryParams(TraceChannel), FCollisionShape::MakeSphere(Radius), CollisionQueryParam
		);

		// 게임 스레드에서 실행될 콜백에 bool 결과만 전달
		FFunctionGraphTask::CreateAndDispatchWhenReady([
			Callback = OnDoneCallback,
			bFound = bFoundSomething
		]()
			{
				Callback(bFound);
			}, TStatId(), nullptr, ENamedThreads::GameThread);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncTargetFindTask, STATGROUP_ThreadPoolAsyncTasks); }

	bool CanAbandon() { return true; }
	void Abandon() {}

private:
	UWorld* World;
	FVector Origin;
	float Radius;
	AActor* SelfActor;
	ECollisionChannel TraceChannel;
	TFunction<void(bool)> OnDoneCallback;
};


UBTService_MTTargetToFind::UBTService_MTTargetToFind()
{
	NodeName = "MT Target Finder"; // 멀티스레드 버전임을 명시
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true; // 각 AI 인스턴스별로 서비스 객체와 메모리를 생성하도록 설정
}

uint16 UBTService_MTTargetToFind::GetInstanceMemorySize() const
{
	return sizeof(FTargetToFindMemory);
}

void UBTService_MTTargetToFind::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// 서비스가 활성화될 때 컴포넌트들을 미리 캐싱하여 성능 향상
	CachedController = OwnerComp.GetAIOwner();
	if (CachedController)
	{
		CachedMobCharacter = Cast<ANonPlayerGASCharacter>(CachedController->GetPawn());
		CachedBlackboard = CachedController->GetBlackboardComponent();
	}
	CachedPlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

}

void UBTService_MTTargetToFind::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(Sync_TargetFinder_Tick);
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FTargetToFindMemory* MyMemory = CastInstanceNodeMemory<FTargetToFindMemory>(NodeMemory);
	if (!MyMemory || MyMemory->bIsTaskInProgress)
	{
		return;
	}

	if (!CachedController || !CachedMobCharacter || !CachedBlackboard)
	{
		return;
	}

	MyMemory->bIsTaskInProgress = true;

	// 콜백 람다가 이제 bool만 받도록 수정됨
	auto OnTaskDoneLambda = [this, Blackboard = CachedBlackboard, MobCharacter = Cast<AMobGASCharacter>(CachedMobCharacter), MyMemory](bool bIsTargetFound)
		{
			// 캐시된 플레이어 포인터가 유효한지 반드시 확인
			if (bIsTargetFound && CachedPlayerCharacter.IsValid())
			{
				AActor* FoundTarget = CachedPlayerCharacter.Get();
				Blackboard->SetValueAsObject(TEXT("Target"), FoundTarget);
				Blackboard->SetValueAsFloat(TEXT("Distance"), CachedMobCharacter->GetDistanceTo(FoundTarget));

				if (MobCharacter) MobCharacter->SetChasingState();
			}
			else
			{
				Blackboard->ClearValue(TEXT("Target"));
				Blackboard->ClearValue(TEXT("Distance"));

				if (MobCharacter) MobCharacter->SetWalkingState();
			}

			MyMemory->bIsTaskInProgress = false;
		};

	(new FAutoDeleteAsyncTask<FAsyncTargetFindTask>(
		GetWorld(),
		CachedMobCharacter->GetActorLocation(),
		Radius,
		CachedMobCharacter,
		ECC_GameTraceChannel2,
		OnTaskDoneLambda
	))->StartBackgroundTask();
}