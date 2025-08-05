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
#include "Attribute/PlayerCharacterAttributeSet.h"
void USkillInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &USkillInfoWidget::CloseUI);
	}
	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &USkillInfoWidget::OnBuyButtonClicked);			//여기서 CallStack
	}
	
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
	//SkillPointCost = SkillData.SkillPointCost;
	SkillID = SkillData.SkillID;

	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FGameplayAttribute SkillPointAttribute = UPlayerCharacterAttributeSet::GetSkillPointAttribute();

	if (SkillPointAttribute.IsValid())
	{
		int32 PlayerSkillPointCost = Player->GetAbilitySystemComponent()->GetNumericAttribute(SkillPointAttribute);
		if (PlayerSkillPointCost < SkillData.SkillPointCost)
		{
			// 스킬 포인트가 부족하면 텍스트를 빨간색으로
			SkillCostText->SetColorAndOpacity(FLinearColor::Red);
		}
		else
		{
			// 충분하면 기본 색상으로
			SkillCostText->SetColorAndOpacity(FLinearColor::White);
		}
	}
}

void USkillInfoWidget::NativeDestruct()
{
	// NativeConstruct에서 했던 바인딩을 여기서 해제
	if (CloseButton)
	{
		CloseButton->OnClicked.Clear(); // Clear()를 사용하여 바인딩을 완전히 제거
	}
	if (BuyButton)
	{
		BuyButton->OnClicked.Clear();
	}

	Super::NativeDestruct(); // 마지막에 부모 함수 호출
}

void USkillInfoWidget::CloseUI()
{
	UE_LOG(LogTemp, Warning, TEXT("SkillInfoWidget: Closing UI"));
	OnSkillInfoClosed.Broadcast();
	SetVisibility(ESlateVisibility::Hidden);
}

void USkillInfoWidget::OnBuyButtonClicked()
{
	AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter->GetSkillTreeComponent()->TryAcquireSkill(SkillID))
	{
		OnSkillInfoClosed.Broadcast();
		SetVisibility(ESlateVisibility::Hidden);

	}

}


