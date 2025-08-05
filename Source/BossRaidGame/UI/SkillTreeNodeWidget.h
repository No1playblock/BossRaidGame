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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeClicked, USkillTreeNodeWidget*, ClickedNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillNodeDoubleClicked, USkillTreeNodeWidget*, DoubleClickedNode); // ����Ŭ�� ��������Ʈ �߰�


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
	UPROPERTY(BlueprintAssignable, Category = "SkillNode")
	FOnSkillNodeClicked OnNodeClicked;

	UPROPERTY(BlueprintAssignable, Category = "SkillNode")
	FOnSkillNodeDoubleClicked OnNodeDoubleClicked;

protected:

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void UpdateNodeVisuals();

protected:

	// �������Ʈ ������ ������Ʈ�� ���ε��� ������
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

	// ���� ����� ��ų ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FSkillTreeDataRow SkillData;


	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor SelectedColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor DefaultColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor LockedColor = FLinearColor(0.18f, 0.18f, 0.18f);

	/** ��ų�� ��� �� ���� ���� �⺻ �׵θ� ���� */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor UnlockedColor = FLinearColor::White;

	/** ��ų ����Ʈ�� ����� ���� �ؽ�Ʈ ���� */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor DefaultCostColor = FLinearColor(FLinearColor::White);

	/** ��ų ����Ʈ�� ������ ���� �ؽ�Ʈ ���� */
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor InsufficientPointsColor = FLinearColor(FLinearColor::Red);


	FSkillTreeDataRow* CachedSkillData;

	// SkillRowHandle�� ��� ������ �������� ���� UI�� ������Ʈ�ϴ� �Լ�

};
