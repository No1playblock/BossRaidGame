// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeNodeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "SkillInfoWidget.h"
#include "SkillSimpleBuyWidget.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"

void USkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

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
			NodeWidget->OnNodeClicked.RemoveAll(this);
			NodeWidget->OnNodeClicked.AddUObject(this, &USkillTreeWidget::HandleNodeSelection);

			NodeWidget->OnNodeDoubleClicked.RemoveAll(this);
			NodeWidget->OnNodeDoubleClicked.AddUObject(this, &USkillTreeWidget::HandleNodeDoubleClick);
		}
	}
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		SkillTreeComp = PlayerCharacter->GetSkillTreeComponent();
		if (SkillTreeComp)
		{
			// 스킬 습득 성공 시 HandleSkillAcquired 함수가 호출되도록 연결
			SkillTreeComp->OnSkillAcquired.RemoveAll(this);
			SkillTreeComp->OnSkillAcquired.AddUObject(this, &USkillTreeWidget::RefreshNodeStates);
		}
	}

	if(SkillInfo)
	{
		SkillInfo->OnSkillInfoClosed.RemoveAll(this);
		SkillInfo->OnSkillInfoClosed.AddUObject(this, &USkillTreeWidget::OnNodeUnSelected);

	}
	
	if (SimpleBuyWidget)
	{
		SimpleBuyWidget->OnSimpleBuyClosed.RemoveAll(this);
		SimpleBuyWidget->OnSimpleBuyClosed.AddUObject(this, &USkillTreeWidget::OnNodeUnSelected);
	}

	if (XBtn)
	{
		XBtn->OnClicked.RemoveDynamic(this, &USkillTreeWidget::OnXButtonClicked);
		XBtn->OnClicked.AddDynamic(this, &USkillTreeWidget::OnXButtonClicked);
	}
	
}

void USkillTreeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	//모든 위젯의 이벤트 바인딩 해제
	for (USkillTreeNodeWidget* NodeWidget : AllSkillNodes)
	{
		if (NodeWidget)
		{
			NodeWidget->OnNodeClicked.Clear();
			NodeWidget->OnNodeDoubleClicked.Clear();
		}
	}
	if (SkillTreeComp)
	{
		SkillTreeComp->OnSkillAcquired.RemoveAll(this);
	}
	SkillInfo->OnSkillInfoClosed.RemoveAll(this);
	SimpleBuyWidget->OnSimpleBuyClosed.RemoveAll(this);
	XBtn->OnClicked.RemoveDynamic(this, &USkillTreeWidget::OnXButtonClicked);
}

FReply USkillTreeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		RemoveFromParent();

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
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleNodeSelection: GetWorld() returned null. Cannot set timer."));
		return;
	}

	if (World->GetTimerManager().IsTimerActive(SelectionTimerHandle))
	{
		return;
	}

	PendingSelectionNode = ClickedNode;

	World->GetTimerManager().SetTimer(SelectionTimerHandle, this, &USkillTreeWidget::ProcessSingleClick, 0.25f, false);
}

void USkillTreeWidget::HandleNodeDoubleClick(USkillTreeNodeWidget* DoubleClickedNode)
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(SelectionTimerHandle);
	PendingSelectionNode = nullptr;
	
	if (CurrentlySelectedNode)
	{
		CurrentlySelectedNode->SetSelected(false);
	}
	CurrentlySelectedNode = DoubleClickedNode;
	CurrentlySelectedNode->SetSelected(true);

	const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();
	if (NodeData)
	{

		SimpleBuyWidget->SetSkillCostText(*NodeData);
		SimpleBuyWidget->SetVisibility(ESlateVisibility::Visible);
		SkillInfo->SetVisibility(ESlateVisibility::Hidden);		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HandleNodeDoubleClick: NodeData is null."));
	}
}

void USkillTreeWidget::RefreshNodeStates()
{
	if (SkillTreeComp)
	{
		for (USkillTreeNodeWidget* Node : AllSkillNodes)
		{
			if (Node)
			{
				Node->UpdateNodeState(SkillTreeComp);
			}
		}
	}

}

void USkillTreeWidget::ProcessSingleClick()  
{  
    if (!PendingSelectionNode.IsValid())  
    {  
		UE_LOG(LogTemp, Warning, TEXT("ProcessSingleClick: PendingSelectionNode is null."));
        return;  
    }  
    if (CurrentlySelectedNode)  
    {  
        CurrentlySelectedNode->SetSelected(false);  
    }  

    if (CurrentlySelectedNode != PendingSelectionNode)  
    {  
        CurrentlySelectedNode = PendingSelectionNode.Get();  
        CurrentlySelectedNode->SetSelected(true);  
        // Fix: Ensure the data is dereferenced properly before passing to OpenSkillInfo  
        const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();  
        if (NodeData)  
        {  
            SkillInfo->OpenSkillInfo(*NodeData);
			SkillInfo->SetVisibility(ESlateVisibility::Visible);
			SimpleBuyWidget->SetVisibility(ESlateVisibility::Hidden);
			
        }  
        else  
        {  
            UE_LOG(LogTemp, Error, TEXT("ProcessSingleClick: NodeData is null."));  
        }  
    }  
}

void USkillTreeWidget::OnNodeUnSelected()
{
	CurrentlySelectedNode->SetSelected(false);
	//PendingSelectionNode = nullptr;
	CurrentlySelectedNode = nullptr;
}

void USkillTreeWidget::OnXButtonClicked()
{
	RemoveFromParent();

#if !(PLATFORM_ANDROID || PLATFORM_IOS)

	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
#endif
}
