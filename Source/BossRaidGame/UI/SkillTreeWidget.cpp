// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeNodeWidget.h"
#include "Kismet/GameplayStatics.h"
void USkillTreeWidget::NativeConstruct()
{
	
	Super::NativeConstruct();
	bIsFocusable = true; // ������ ��Ŀ���� ���� �� �ֵ��� ����
}


FReply USkillTreeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		// Tab Ű�� ������ ���� ������ �����մϴ� (��: ���� �ݱ�)
		RemoveFromParent();
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController)
		{
			// �Է� ��带 �������� �ǵ����� ���콺 Ŀ���� ����ϴ�.
			PlayerController->SetShowMouseCursor(false);
			PlayerController->SetInputMode(FInputModeGameOnly());

		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
