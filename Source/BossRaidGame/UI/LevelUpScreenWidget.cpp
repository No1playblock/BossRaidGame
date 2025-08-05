#include "UI/LevelUpScreenWidget.h"
#include "UI/StatChoiceCardWidget.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Subsystems/LevelUpSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

void ULevelUpScreenWidget::InitializeChoices(const TArray<FStatChoiceInfo>& Choices, AGASCharacterPlayer* InPlayerOwner)
{
	PlayerOwner = InPlayerOwner;

	if (!ChoiceContainer || !ChoiceCardClass || !PlayerOwner)
	{
		return;
	}

	ChoiceContainer->ClearChildren();

	for (const FStatChoiceInfo& ChoiceInfo : Choices)
	{
		UStatChoiceCardWidget* CardWidget = CreateWidget<UStatChoiceCardWidget>(this, ChoiceCardClass);
		if (CardWidget)
		{
			CardWidget->SetChoiceInfo(ChoiceInfo);

			CardWidget->OnChoiceCardSelected.AddDynamic(this, &ULevelUpScreenWidget::OnCardSelected);

			UHorizontalBoxSlot* NewSlot = ChoiceContainer->AddChildToHorizontalBox(CardWidget);
			if (NewSlot)
			{
				NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center); 
				NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

				FSlateChildSize NewSize;
				NewSize.SizeRule = ESlateSizeRule::Fill;  // Fill�� ����
				NewSize.Value = 1.0f;

				NewSlot->SetSize(NewSize);
				NewSlot->SetPadding(FMargin(15.f, 0.f));                   
			}
		}
	}
}

void ULevelUpScreenWidget::OnCardSelected(const FStatChoiceInfo& SelectedChoice)
{

	if (PlayerOwner)
	{
		APlayerController* PC = PlayerOwner->GetController<APlayerController>();
		if (PC)
		{
			ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
			if (LocalPlayer)
			{
				ULevelUpSubsystem* LevelUpSubsystem = LocalPlayer->GetSubsystem<ULevelUpSubsystem>();
				if (LevelUpSubsystem)
				{
					// ����ý��� �Լ� ȣ��
					LevelUpSubsystem->ApplyStatChoice(SelectedChoice);
				}
			}
		}
	}
}