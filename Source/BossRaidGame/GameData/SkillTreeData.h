#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h" // FGameplayTag
#include "Abilities/GameplayAbility.h"
#include "SkillTreeData.generated.h"

USTRUCT(BlueprintType)
struct FSkillTreeDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// �� ��ų�� ���� ID (��: "Player.Skill.Fireball.Tier1")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillID;

	// �� ��ų�� ���� ���� (��: Gameplay.Skill.Q) - ��ų�� �����ϱ� ���� �±�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillSlotTag;

	// UI�� ǥ�õ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText UpgradeDescription; // �̹� ���׷��̵忡 ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> SkillIcon;

	// ���׷��̵忡 �ʿ��� ��ų ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillPointCost;

	// �� ��ų�� ����� �� �ο��� ���� GameplayAbility Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> GrantedAbility;

	// --- ��ȭ �� �б� ���� ---

	// �� ���׷��̵尡 �б������� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBranchingPoint;

	// �Ϲ����� ���� �ܰ� ��ų�� ID (bIsBranchingPoint�� false�� �� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bIsBranchingPoint"))
	FName NextLevelSkillID;

	// �б� ������ ��� (bIsBranchingPoint�� true�� �� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsBranchingPoint"))
	TArray<FName> BranchChoices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D UIPosition;
};