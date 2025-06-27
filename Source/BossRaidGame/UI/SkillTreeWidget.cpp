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
	bIsFocusable = true; // 위젯이 포커스를 받을 수 있도록 설정
}


FReply USkillTreeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		// Tab 키가 눌렸을 때의 로직을 실행합니다 (예: 위젯 닫기)
		RemoveFromParent();
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController)
		{
			// 입력 모드를 게임으로 되돌리고 마우스 커서를 숨깁니다.
			PlayerController->SetShowMouseCursor(false);
			PlayerController->SetInputMode(FInputModeGameOnly());

		}

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
