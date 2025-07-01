// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeNodeWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameData/SkillTreeData.h"
#include "Blueprint/WidgetTree.h" // WidgetTree를 사용하기 위해 포함


void USkillTreeNodeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateNodeVisuals();

}

FReply USkillTreeNodeWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnNodeDoubleClicked.Broadcast(this);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply USkillTreeNodeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnNodeClicked.Broadcast(this);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}


void USkillTreeNodeWidget::SetSelected(bool bIsSelected)
{
	if (NodeBorder)
	{
		NodeBorder->SetBrushColor(bIsSelected ? SelectedColor : DefaultColor);
	}
}


void USkillTreeNodeWidget::SetSkillData(const FSkillTreeDataRow& InData)
{
    SkillData = InData;
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
			FText::FromString(TEXT("Cost: {0}")),
			FText::AsNumber(SkillData.SkillPointCost)
		
		));
	}
	if (SkillDamageText)
	{
		SkillDamageText->SetText(FText::Format(
			FText::FromString(TEXT("Damage: {0}")),
			FText::AsNumber(SkillData.SkillDamage)
		));
	}
    if (SkillCoolTimeText)
    {
		SkillCoolTimeText->SetText(FText::Format(
			FText::FromString(TEXT("CoolTime: {0}")),
			FText::AsNumber(SkillData.SkillCoolTime)
		));
    }
    if (SkillDescriptionText)
    {
        SkillDescriptionText->SetText(SkillData.UpgradeDescription);
    }
	UE_LOG(LogTemp, Warning, TEXT("Success"));

}
void USkillTreeNodeWidget::UpdateNodeVisuals()
{

	if (!SkillRowHandle.DataTable || SkillRowHandle.RowName == NAME_None)
	{
		CachedSkillData = nullptr;
		return;
	}
	// 데이터 테이블에서 RowName에 해당하는 데이터를 찾아 로드합니다.
	const FString ContextString(TEXT("USkillNodeWidget::UpdateNodeVisuals"));
	CachedSkillData = SkillRowHandle.GetRow<FSkillTreeDataRow>(ContextString);
	SkillData = *CachedSkillData; // 캐시된 데이터를 현재 SkillData에 저장
	SetSkillData(*CachedSkillData); // UI 위젯들을 업데이트합니다.
	
}




