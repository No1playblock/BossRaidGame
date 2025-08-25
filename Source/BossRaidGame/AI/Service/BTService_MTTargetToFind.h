#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_MTTargetToFind.generated.h"

class UBlackboardComponent;
class ANonPlayerGASCharacter;
struct FTargetScanInput;
/**
 * ��Ƽ������ ��ó�� ��� Ÿ�� Ž�� ����
 * - Tick ������ �ĺ��� �������� ���� ��Ŀ ������� �ѱ�
 * - ���� ƽ���� ��� ť�� �����Ͽ� ������ ����(GameThread)
 * - ���� ������ ���� ����(������ ������ ����)
 */

struct FTargetToFindMemory
{
	// �񵿱� �۾��� ���� ���� ������ ����
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

	// �� ���񽺰� ����� �޸� ����ü�� ũ�⸦ ������ �˷���
	virtual uint16 GetInstanceMemorySize() const override;

	UPROPERTY(EditAnywhere, Category = "Config")
	float Radius = 1500.0f;

	// ���� ����ϴ� ������Ʈ�� ĳ���ϱ� ���� ����
	UPROPERTY()
	TObjectPtr<AAIController> CachedController;
	UPROPERTY()
	TObjectPtr<class ANonPlayerGASCharacter> CachedMobCharacter;
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> CachedBlackboard;

	TWeakObjectPtr<ACharacter> CachedPlayerCharacter;

};
