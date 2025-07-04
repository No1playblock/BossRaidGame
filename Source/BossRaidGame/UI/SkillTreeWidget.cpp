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
#include "SkillInfoWidget.h"
#include "SkillSimpleBuyWidget.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"

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
	//RefreshNodeStates();
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
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		SkillTreeComp = PlayerCharacter->GetSkillTreeComponent();
		if (SkillTreeComp)
		{
			//UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget: Found SkillTreeComponent for player %s"), *PlayerCharacter->GetName());
			// ��ų ���� ���� �� HandleSkillAcquired �Լ��� ȣ��ǵ��� �����մϴ�.
			SkillTreeComp->OnSkillAcquired.AddDynamic(this, &USkillTreeWidget::RefreshNodeStates);
		}
	}
	SkillInfo->OnSkillInfoClosed.AddDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
	SimpleBuyWidget->OnSimpleBuyClosed.AddDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
}


FReply USkillTreeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		RemoveFromParent();
		//UGameplayStatics::SetGamePaused(GetWorld(), false);
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

	// �̹� Ÿ�̸Ӱ� ���� ���̸� (������ ���� �� Ŭ��), ���ο� Ÿ�̸Ӹ� �������� �ʽ��ϴ�.
	if (World->GetTimerManager().IsTimerActive(SelectionTimerHandle))
	{
		return;
	}

	PendingSelectionNode = ClickedNode;

	// 0.25�� �ڿ� ProcessSingleClick �Լ��� ȣ���ϴ� Ÿ�̸Ӹ� �����մϴ�.
	World->GetTimerManager().SetTimer(SelectionTimerHandle, this, &USkillTreeWidget::ProcessSingleClick, 0.25f, false);
}

void USkillTreeWidget::HandleNodeDoubleClick(USkillTreeNodeWidget* DoubleClickedNode)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// ����Ŭ���� �߻������Ƿ�, ��� ���̴� ���� Ŭ�� Ÿ�̸Ӹ� ����մϴ�.
	World->GetTimerManager().ClearTimer(SelectionTimerHandle);
	PendingSelectionNode = nullptr; // ��� ���� ��嵵 ���ϴ�.
	
	UE_LOG(LogTemp, Warning, TEXT("HandleNodeDoubleClick: Double clicked on node %s"), *DoubleClickedNode->GetName());
	if (CurrentlySelectedNode)
	{
		CurrentlySelectedNode->SetSelected(false);
	}
	CurrentlySelectedNode = DoubleClickedNode;
	CurrentlySelectedNode->SetSelected(true);
	//�����Ͻðڽ��ϱ�?
	const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();
	if (NodeData)
	{

		SimpleBuyWidget->SetSkillCostText(*NodeData);
		SimpleBuyWidget->SetVisibility(ESlateVisibility::Visible);

		UE_LOG(LogTemp, Warning, TEXT("HandleNodeDoubleClick: Opened simple buy widget for node %s"), *CurrentlySelectedNode->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HandleNodeDoubleClick: NodeData is null."));
	}
}

//��ų�� ���ų� ���� ������ ��带 ����
void USkillTreeWidget::RefreshNodeStates()
{
	//UE_LOG(LogTemp, Warning, TEXT("RefreshNodeStates: Refreshing all skill nodes"));
	if (SkillTreeComp)
	{
		for (USkillTreeNodeWidget* Node : AllSkillNodes)
		{
			if (Node)
			{
				//UE_LOG(LogTemp, Warning, TEXT("RefreshNodeStates: Updating node %s"), *Node->GetName());
				Node->UpdateNodeState(SkillTreeComp);
			}
		}
	}

}

void USkillTreeWidget::ProcessSingleClick()  
{  
    if (!PendingSelectionNode)  
    {  
        return;  
    }  
    if (CurrentlySelectedNode)  
    {  
        CurrentlySelectedNode->SetSelected(false);  
    }  

    UE_LOG(LogTemp, Warning, TEXT("ProcessSingleClick: Selected node %s"), *PendingSelectionNode->GetName());  
    if (CurrentlySelectedNode != PendingSelectionNode)  
    {  
        CurrentlySelectedNode = PendingSelectionNode;  
        CurrentlySelectedNode->SetSelected(true);  

        // Fix: Ensure the data is dereferenced properly before passing to OpenSkillInfo  
        const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();  
        if (NodeData)  
        {  
            SkillInfo->OpenSkillInfo(*NodeData);
			SkillInfo->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("ProcessSingleClick: Opened skill info for node %s"), *CurrentlySelectedNode->GetName());
			
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
	//CurrentlySelectedNode = nullptr;
}

