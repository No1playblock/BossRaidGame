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
void USkillSimpleBuyWidget::NativeDestruct()
{
	if(BuyButton)
	{
		BuyButton->OnClicked.Clear();
	}
	if(NoButton)
	{
		NoButton->OnClicked.Clear();
	}



	Super::NativeDestruct();


}
void USkillSimpleBuyWidget::OnBuyButtonClicked()
{
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!PlayerCharacter) return;
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
