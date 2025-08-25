#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MTTargetToFind.generated.h"

class UBlackboardComponent;
class ANonPlayerGASCharacter;
struct FTargetScanInput;
/**
 * 멀티스레드 전처리 기반 타겟 탐색 서비스
 * - Tick 시점에 후보군 스냅샷을 떠서 워커 스레드로 넘김
 * - 다음 틱에서 결과 큐를 폴링하여 블랙보드 갱신(GameThread)
 * - 물리 쿼리는 하지 않음(스레드 세이프 보장)
 */

struct FTargetToFindMemory
{
	// 비동기 작업이 현재 실행 중인지 여부
	bool bIsTaskInProgress = false;
};


UCLASS()
class BOSSRAIDGAME_API UBTService_MTTargetToFind : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_MTTargetToFind();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 이 서비스가 사용할 메모리 구조체의 크기를 엔진에 알려줌
	virtual uint16 GetInstanceMemorySize() const override;

	UPROPERTY(EditAnywhere, Category = "Config")
	float Radius = 1500.0f;

	// 자주 사용하는 컴포넌트를 캐싱하기 위한 변수
	UPROPERTY()
	TObjectPtr<AAIController> CachedController;
	UPROPERTY()
	TObjectPtr<class ANonPlayerGASCharacter> CachedMobCharacter;
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> CachedBlackboard;

	TWeakObjectPtr<ACharacter> CachedPlayerCharacter;

};
