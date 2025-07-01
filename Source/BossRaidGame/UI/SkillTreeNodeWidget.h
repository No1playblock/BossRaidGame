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
class USkillTreeNodeWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeClicked, USkillTreeNodeWidget*, ClickedNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeDoubleClicked, USkillTreeNodeWidget*, DoubleClickedNode); // 더블클릭 델리게이트 추가


UCLASS()
class BOSSRAIDGAME_API USkillTreeNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillData(const FSkillTreeDataRow& InData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FDataTableRowHandle SkillRowHandle;

	// 외부에서 이 노드의 스킬 데이터를 가져갈 수 있도록 Getter를 제공합니다.
	FORCEINLINE const FSkillTreeDataRow* GetNodeData() const { return CachedSkillData; }

	void SetSelected(bool bIsSelected);

public:
	UPROPERTY(BlueprintAssignable, Category = "SkillNode")
	FOnSkillNodeClicked OnNodeClicked;

	UPROPERTY(BlueprintAssignable, Category = "SkillNode")
	FOnSkillNodeDoubleClicked OnNodeDoubleClicked;

protected:

	virtual void NativePreConstruct() override;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void UpdateNodeVisuals();

protected:

	// 블루프린트 위젯의 컴포넌트와 바인딩될 변수들
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBorder> NodeBorder;

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


	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor SelectedColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor DefaultColor = FLinearColor::White;

	FSkillTreeDataRow* CachedSkillData;

	// SkillRowHandle에 담긴 정보를 바탕으로 실제 UI를 업데이트하는 함수

};
