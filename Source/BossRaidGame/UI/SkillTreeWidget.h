// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTreeWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API USkillTreeWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	// SkillTree DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillTree")
	UDataTable* SkillTreeDataTable;

	// UI 배치용 캔버스
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> SkillTreeCanvas;

	// 노드 위젯 클래스 (디자인용)
	UPROPERTY(EditDefaultsOnly, Category = "SkillTree")
	TSubclassOf<class USkillTreeNodeWidget> SkillTreeNodeWidgetClass;

	//void CloseWidget();



protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

};
