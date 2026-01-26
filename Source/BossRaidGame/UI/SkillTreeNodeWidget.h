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
class USkillTreeComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillNodeClicked, USkillTreeNodeWidget*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillNodeDoubleClicked, USkillTreeNodeWidget*); // 더블클릭 델리게이트 추가


UCLASS()
class BOSSRAIDGAME_API USkillTreeNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillData(const FSkillTreeDataRow& InData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FDataTableRowHandle SkillRowHandle;

	FORCEINLINE const FSkillTreeDataRow* GetNodeData() const { return CachedSkillData; }

	void SetSelected(bool bIsSelected);

	void UpdateNodeState(USkillTreeComponent* SkillTreeComp);

public:
	FOnSkillNodeClicked OnNodeClicked;

	FOnSkillNodeDoubleClicked OnNodeDoubleClicked;

protected:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
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

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor LockedColor = FLinearColor(0.18f, 0.18f, 0.18f);

	/** 스킬을 배울 수 있을 때의 기본 테두리 색상 */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor UnlockedColor = FLinearColor::White;

	/** 스킬 포인트가 충분할 때의 텍스트 색상 */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor DefaultCostColor = FLinearColor(FLinearColor::White);

	/** 스킬 포인트가 부족할 때의 텍스트 색상 */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor InsufficientPointsColor = FLinearColor(FLinearColor::Red);


	FSkillTreeDataRow* CachedSkillData;

	// SkillRowHandle에 담긴 정보를 바탕으로 실제 UI를 업데이트하는 함수

};
