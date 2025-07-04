// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSimpleBuyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GASCharacterPlayer.h"
#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"
void USkillSimpleBuyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &USkillSimpleBuyWidget::OnBuyButtonClicked);
	}
	if (NoButton)
	{
		NoButton->OnClicked.AddDynamic(this, &USkillSimpleBuyWidget::OnNoButtonClicked);
	}
}

void USkillSimpleBuyWidget::OnNoButtonClicked()
{
	OnSimpleBuyClosed.Broadcast();
	SetVisibility(ESlateVisibility::Hidden);
}

void USkillSimpleBuyWidget::SetSkillCostText(const FSkillTreeDataRow& SkillData)
{
	if (SkillPointText)
	{
		SkillPointText->SetText(FText::Format(
			FText::FromString(TEXT("Cost : {0}")),
			FText::AsNumber(SkillData.SkillPointCost)

		));
	}
	SkillID = SkillData.SkillID;
}
void USkillSimpleBuyWidget::OnBuyButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("SkillInfoWidget: Buy button clicked for skill %s"), *SkillID.ToString());
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter->GetSkillTreeComponent()->TryAcquireSkill(SkillID))
	{
		OnSimpleBuyClosed.Broadcast();
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SkillPointText->SetColorAndOpacity(FLinearColor::Red);
	}
	

}
