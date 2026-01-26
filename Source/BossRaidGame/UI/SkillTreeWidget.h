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

	// UI 배치용 캔버스
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


	UFUNCTION()
	void OnXButtonClicked();
protected:


	// 등록된 모든 스킬 노드 위젯의 목록
	UPROPERTY()
	TArray<USkillTreeNodeWidget*> AllSkillNodes;

	// 현재 선택된 노드를 가리키는 포인터
	UPROPERTY()
	USkillTreeNodeWidget* CurrentlySelectedNode;


	FTimerHandle SelectionTimerHandle;

	UPROPERTY()
	TWeakObjectPtr<USkillTreeNodeWidget> PendingSelectionNode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class USkillInfoWidget> SkillInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USkillSimpleBuyWidget> SimpleBuyWidget;

	TObjectPtr<class USkillTreeComponent> SkillTreeComp;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> XBtn;
};
