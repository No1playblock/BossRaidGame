#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h" // FGameplayTag
#include "Abilities/GameplayAbility.h"
#include "SkillTreeData.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None		UMETA(DisplayName = "None"),
	// GASCharacterPlayer�� InputID�� ���� ��ġ���Ѿ� ��.
	Q_Skill = 2	UMETA(DisplayName = "Q Skill"),      // 2
	E_Skill			UMETA(DisplayName = "E Skill"),      // 3
	R_Skill			UMETA(DisplayName = "R Skill"),      // 4
	Shift_Skill		UMETA(DisplayName = "Shift Skill")   // 5
};


USTRUCT(BlueprintType)
struct FSkillTreeDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// �� ��ų�� ���� ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillID;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PrerequisiteSkillID;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillCoolTime;

	// �� ��ų�� ����� �� �ο��� ���� GameplayAbility Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> GrantedAbility;



	// �� ���׷��̵尡 �б������� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBranchingPoint;

	// �Ϲ����� ���� �ܰ� ��ų�� ID (bIsBranchingPoint�� false�� �� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bIsBranchingPoint"))
	FName NextLevelSkillID;

	// �б� ������ ��� (bIsBranchingPoint�� true�� �� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsBranchingPoint"))
	TArray<FName> BranchChoices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAbilityInputID InputID = EAbilityInputID::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D UIPosition;


};