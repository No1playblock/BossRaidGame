// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

/**
 * 
 */
class USkillTreeNodeWidget;
UCLASS()
class BOSSRAIDGAME_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()


public:


	// SkillTree DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillTree")
	UDataTable* SkillTreeDataTable;

	// UI ��ġ�� ĵ����
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> SkillTreeCanvas;

	UFUNCTION()
	void RefreshNodeStates();

protected:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;	

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void HandleNodeSelection(USkillTreeNodeWidget* ClickedNode);

	UFUNCTION()
	void HandleNodeDoubleClick(USkillTreeNodeWidget* DoubleClickedNode);

	void ProcessSingleClick();

	UFUNCTION()
	void OnNodeUnSelected();

protected:
	// ��ϵ� ��� ��ų ��� ������ ���
	UPROPERTY()
	TArray<USkillTreeNodeWidget*> AllSkillNodes;

	// ���� ���õ� ��带 ����Ű�� ������
	UPROPERTY()
	USkillTreeNodeWidget* CurrentlySelectedNode;


	FTimerHandle SelectionTimerHandle;

	UPROPERTY()
	TObjectPtr<USkillTreeNodeWidget> PendingSelectionNode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USkillInfoWidget> SkillInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USkillSimpleBuyWidget> SimpleBuyWidget;

	TObjectPtr<class USkillTreeComponent> SkillTreeComp;
};
