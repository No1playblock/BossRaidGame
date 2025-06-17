#include "UI/StatChoiceCardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UStatChoiceCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UStatChoiceCardWidget::OnSelectButtonClicked);
	}
}

void UStatChoiceCardWidget::SetChoiceInfo(const FStatChoiceInfo& InChoiceInfo)
{
	CurrentChoiceInfo = InChoiceInfo;

	if (StatNameText)
	{
		StatNameText->SetText(CurrentChoiceInfo.DisplayName);
	}
	if (StatValueText)
	{
		StatValueText->SetText(CurrentChoiceInfo.DisplayValue);
	}
	if (StatLevelText)
	{
		StatLevelText->SetText(CurrentChoiceInfo.DisplayLevelText);
	}
}

void UStatChoiceCardWidget::OnSelectButtonClicked()
{
	OnChoiceCardSelected.Broadcast(CurrentChoiceInfo);
}