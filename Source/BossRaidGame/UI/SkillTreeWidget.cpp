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
	bIsFocusable = true; // 위젯이 포커스를 받을 수 있도록 설정

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
			// 스킬 습득 성공 시 HandleSkillAcquired 함수가 호출되도록 연결
			SkillTreeComp->OnSkillAcquired.AddDynamic(this, &USkillTreeWidget::RefreshNodeStates);
		}
	}
	SkillInfo->OnSkillInfoClosed.AddDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
	SimpleBuyWidget->OnSimpleBuyClosed.AddDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
	XBtn->OnClicked.AddDynamic(this, &USkillTreeWidget::OnXButtonClicked);
}

void USkillTreeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	// 모든 노드 위젯의 이벤트 바인딩을 해제
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
		SkillTreeComp->OnSkillAcquired.RemoveDynamic(this, &USkillTreeWidget::RefreshNodeStates);
	}
	SkillInfo->OnSkillInfoClosed.RemoveDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
	SimpleBuyWidget->OnSimpleBuyClosed.RemoveDynamic(this, &USkillTreeWidget::OnNodeUnSelected);
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

	// 이미 타이머가 실행 중이면 (빠르게 여러 번 클릭), 새로운 타이머를 시작 안함
	if (World->GetTimerManager().IsTimerActive(SelectionTimerHandle))
	{
		return;
	}

	PendingSelectionNode = ClickedNode;

	// 0.25초 뒤에 ProcessSingleClick 함수를 호출하는 타이머 설정
	World->GetTimerManager().SetTimer(SelectionTimerHandle, this, &USkillTreeWidget::ProcessSingleClick, 0.25f, false);
}

void USkillTreeWidget::HandleNodeDoubleClick(USkillTreeNodeWidget* DoubleClickedNode)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 더블클릭이 발생했으므로, 대기 중이던 단일 클릭 타이머 취소
	World->GetTimerManager().ClearTimer(SelectionTimerHandle);
	PendingSelectionNode = nullptr; // 대기 중인 노드도 비움
	
	UE_LOG(LogTemp, Warning, TEXT("HandleNodeDoubleClick: Double clicked on node %s"), *DoubleClickedNode->GetName());
	if (CurrentlySelectedNode)
	{
		CurrentlySelectedNode->SetSelected(false);
	}
	CurrentlySelectedNode = DoubleClickedNode;
	CurrentlySelectedNode->SetSelected(true);
	//구매하시겠습니까?
	const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();
	if (NodeData)
	{

		SimpleBuyWidget->SetSkillCostText(*NodeData);
		SimpleBuyWidget->SetVisibility(ESlateVisibility::Visible);
		SkillInfo->SetVisibility(ESlateVisibility::Hidden);		//다른거는 안보이게

		UE_LOG(LogTemp, Warning, TEXT("HandleNodeDoubleClick: Opened simple buy widget for node %s"), *CurrentlySelectedNode->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HandleNodeDoubleClick: NodeData is null."));
	}
}

//스킬을 배우거나 최초 생성시 노드 갱신
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
		UE_LOG(LogTemp, Warning, TEXT("ProcessSingleClick: PendingSelectionNode is null."));
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
		UE_LOG(LogTemp, Warning, TEXT("ProcessSingleClick: CurrentlySelectedNode set to %s"), *CurrentlySelectedNode->GetName());
        // Fix: Ensure the data is dereferenced properly before passing to OpenSkillInfo  
        const FSkillTreeDataRow* NodeData = CurrentlySelectedNode->GetNodeData();  
        if (NodeData)  
        {  
            SkillInfo->OpenSkillInfo(*NodeData);
			SkillInfo->SetVisibility(ESlateVisibility::Visible);
			SimpleBuyWidget->SetVisibility(ESlateVisibility::Hidden);	//다른거는 안보이게
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
	//PendingSelectionNode = nullptr;
	CurrentlySelectedNode = nullptr;
}

void USkillTreeWidget::OnXButtonClicked()
{
	RemoveFromParent();
	//UGameplayStatics::SetGamePaused(GetWorld(), false);
	APlayerController* PlayerController = GetOwningPlayer();
#if !(PLATFORM_ANDROID || PLATFORM_IOS)
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
#endif
}
