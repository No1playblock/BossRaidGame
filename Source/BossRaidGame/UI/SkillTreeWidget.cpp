// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeNodeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h" // WidgetTree�� ����ϱ� ���� �ʿ��մϴ�.


void USkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true; // ������ ��Ŀ���� ���� �� �ֵ��� ����

	if (WidgetTree)
	{
		TArray<UWidget*> AllWidgets;
		WidgetTree->GetAllWidgets(AllWidgets);

		for (UWidget* Widget : AllWidgets)
		{
			if (USkillTreeNodeWidget* NodeWidget = Cast<USkillTreeNodeWidget>(Widget))
			{
				AllSkillNodes.Add(NodeWidget);
			}
		}
	}

	for (USkillTreeNodeWidget* NodeWidget : AllSkillNodes)
	{
		if (NodeWidget)
		{
			if (!NodeWidget->OnNodeClicked.IsAlreadyBound(this, &USkillTreeWidget::HandleNodeSelection))
			{
				NodeWidget->OnNodeClicked.AddDynamic(this, &USkillTreeWidget::HandleNodeSelection);
			}
			if (!NodeWidget->OnNodeDoubleClicked.IsAlreadyBound(this, &USkillTreeWidget::HandleNodeDoubleClick))
			{
				NodeWidget->OnNodeDoubleClicked.AddDynamic(this, &USkillTreeWidget::HandleNodeDoubleClick);
			}
		}
	}
}




FReply USkillTreeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		RemoveFromParent();
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController)
		{
			PlayerController->SetShowMouseCursor(false);
			PlayerController->SetInputMode(FInputModeGameOnly());

		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USkillTreeWidget::HandleNodeSelection(USkillTreeNodeWidget* ClickedNode)
{
	UE_LOG(LogTemp, Warning, TEXT("OneClick"));
	if (CurrentlySelectedNode)
	{
		CurrentlySelectedNode->SetSelected(false);
	}

	if (CurrentlySelectedNode != ClickedNode)
	{
		CurrentlySelectedNode = ClickedNode;
		CurrentlySelectedNode->SetSelected(true);
	}
	else
	{
		CurrentlySelectedNode = nullptr;
	}
}

void USkillTreeWidget::HandleNodeDoubleClick(USkillTreeNodeWidget* DoubleClickedNode)
{
	UE_LOG(LogTemp, Warning, TEXT("DoubleClick"));

	if (CurrentlySelectedNode && CurrentlySelectedNode == DoubleClickedNode)
	{
		// ���� ���õ� ��尡 ����Ŭ���Ǹ� ���� ����
		CurrentlySelectedNode->SetSelected(true);
		CurrentlySelectedNode = DoubleClickedNode;
	}
	else
	{
		// ����Ŭ���� ��带 ����
		HandleNodeSelection(DoubleClickedNode);
	}
}
