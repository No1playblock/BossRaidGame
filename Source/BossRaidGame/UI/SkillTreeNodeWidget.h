// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeNodeWidget.generated.h"


class UImage;
class UTextBlock;
class UBorder;
class UButton;
UCLASS()
class BOSSRAIDGAME_API USkillTreeNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 스킬 데이터를 설정하고 UI를 업데이트합니다.
	void SetSkillData(const FSkillTreeDataRow& InData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FDataTableRowHandle SkillRowHandle;

	// 외부에서 이 노드의 스킬 데이터를 가져갈 수 있도록 Getter를 제공합니다.
	FORCEINLINE const FSkillTreeDataRow* GetNodeData() const { return CachedSkillData; }

protected:

	virtual void NativePreConstruct() override;

	void UpdateNodeVisuals();


	// 블루프린트 위젯의 컴포넌트와 바인딩될 변수들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> NodeBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkillButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDamageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillCoolTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillCostText;

	// 현재 노드의 스킬 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FSkillTreeDataRow SkillData;

	FSkillTreeDataRow* CachedSkillData;

	// SkillRowHandle에 담긴 정보를 바탕으로 실제 UI를 업데이트하는 함수

};
