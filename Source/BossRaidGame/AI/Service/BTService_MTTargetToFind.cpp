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

		// ��ġ�� ���� �ִ��� '����'�� ������ Ȯ��
		bool bFoundSomething = World->OverlapMultiByObjectType(
			OverlapResults, Origin, FQuat::Identity, FCollisionObjectQueryParams(TraceChannel), FCollisionShape::MakeSphere(Radius), CollisionQueryParam
		);

		// ���� �����忡�� ����� �ݹ鿡 bool ����� ����
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
	NodeName = "MT Target Finder"; // ��Ƽ������ �������� ���
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true; // �� AI �ν��Ͻ����� ���� ��ü�� �޸𸮸� �����ϵ��� ����
}

uint16 UBTService_MTTargetToFind::GetInstanceMemorySize() const
{
	return sizeof(FTargetToFindMemory);
}

void UBTService_MTTargetToFind::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// ���񽺰� Ȱ��ȭ�� �� ������Ʈ���� �̸� ĳ���Ͽ� ���� ���
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

	// �ݹ� ���ٰ� ���� bool�� �޵��� ������
	auto OnTaskDoneLambda = [this, Blackboard = CachedBlackboard, MobCharacter = Cast<AMobGASCharacter>(CachedMobCharacter), MyMemory](bool bIsTargetFound)
		{
			// ĳ�õ� �÷��̾� �����Ͱ� ��ȿ���� �ݵ�� Ȯ��
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