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
	// GASCharacterPlayer의 InputID와 값을 일치시켜야 합니다.
	Q_Skill = 2	UMETA(DisplayName = "Q Skill"),      // 2
	E_Skill			UMETA(DisplayName = "E Skill"),      // 3
	R_Skill			UMETA(DisplayName = "R Skill"),      // 4
	Shift_Skill		UMETA(DisplayName = "Shift Skill")   // 5
};


USTRUCT(BlueprintType)
struct FSkillTreeDataRow : public FTableRowBase
{
	GENERATED_BODY()

	// 이 스킬의 고유 ID (예: "Player.Skill.Fireball.Tier1")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillID;

	// 이 스킬이 속한 슬롯 (예: Gameplay.Skill.Q) - 스킬을 구분하기 위한 태그
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillSlotTag;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PrerequisiteSkillID;

	// UI에 표시될 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText UpgradeDescription; // 이번 업그레이드에 대한 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> SkillIcon;

	// 업그레이드에 필요한 스킬 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillPointCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillCoolTime;

	// 이 스킬을 배웠을 때 부여될 실제 GameplayAbility 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> GrantedAbility;

	// --- 진화 및 분기 정보 ---

	// 이 업그레이드가 분기점인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBranchingPoint;

	// 일반적인 다음 단계 스킬의 ID (bIsBranchingPoint가 false일 때 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bIsBranchingPoint"))
	FName NextLevelSkillID;

	// 분기 선택지 목록 (bIsBranchingPoint가 true일 때 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsBranchingPoint"))
	TArray<FName> BranchChoices;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAbilityInputID InputID = EAbilityInputID::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D UIPosition;


};