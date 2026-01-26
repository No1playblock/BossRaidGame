// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameOverWidget.h"
#include "Components/Button.h"
#include "Player/BRPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼과 함수 연결 (바인딩)
	if (YesBtn)
	{
		YesBtn->OnClicked.AddDynamic(this, &UGameOverWidget::OnYesBtnClicked);
	}

	if (NoBtn)
	{
		NoBtn->OnClicked.AddDynamic(this, &UGameOverWidget::OnNoBtnClicked);
	}
}

void UGameOverWidget::OnYesBtnClicked()
{
	if (ABRPlayerController* PC = Cast<ABRPlayerController>(GetOwningPlayer()))
	{
		PC->RequestRestartGame();
	}
}

void UGameOverWidget::OnNoBtnClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}