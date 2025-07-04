// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInfoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameData/SkillTreeData.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GASCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "Components/SkillTreeComponent.h"
#include "SkillSimpleBuyWidget.h"
void USkillInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CloseButton->OnClicked.AddDynamic(this, &USkillInfoWidget::CloseUI);
	BuyButton->OnClicked.AddDynamic(this, &USkillInfoWidget::OnBuyButtonClicked);
}

void USkillInfoWidget::OpenSkillInfo(const FSkillTreeDataRow& SkillData)
{
	//FSkillTreeDataRow
	if (SkillNameText)
	{
		SkillNameText->SetText(SkillData.SkillName);
	}
	if (SkillIcon && SkillData.SkillIcon.IsValid())
	{
		SkillIcon->SetBrushFromTexture(SkillData.SkillIcon.LoadSynchronous());
	}

	if (SkillCostText)
	{
		SkillCostText->SetText(FText::Format(
			FText::FromString(TEXT("Cost({0}) : Buy")),
			FText::AsNumber(SkillData.SkillPointCost)

		));
	}
	if (DamageText)
	{
		DamageText->SetText(FText::Format(
			FText::FromString(TEXT("Damage: {0}")),
			FText::AsNumber(SkillData.SkillDamage)
		));
	}
	if (CoolTimeText)
	{
		CoolTimeText->SetText(FText::Format(
			FText::FromString(TEXT("CoolTime: {0}")),
			FText::AsNumber(SkillData.SkillCoolTime)
		));
	}
	if (DescriptionText)
	{
		DescriptionText->SetText(SkillData.UpgradeDescription);
	}
	SkillPointCost = SkillData.SkillPointCost;
	SkillID = SkillData.SkillID;
}

void USkillInfoWidget::CloseUI()
{
	UE_LOG(LogTemp, Warning, TEXT("SkillInfoWidget: Closing UI"));
	OnSkillInfoClosed.Broadcast();
	SetVisibility(ESlateVisibility::Hidden);
}

void USkillInfoWidget::OnBuyButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("SkillInfoWidget: Buy button clicked for skill %s"), *SkillID.ToString());	
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter->GetSkillTreeComponent()->TryAcquireSkill(SkillID))
	{
		OnSkillInfoClosed.Broadcast();
		SetVisibility(ESlateVisibility::Hidden);

	}
	else
	{
		SkillCostText->SetColorAndOpacity(FLinearColor::Red);

	}

}


