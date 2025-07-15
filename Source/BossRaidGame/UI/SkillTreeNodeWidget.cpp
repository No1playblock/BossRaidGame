// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeNodeWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameData/SkillTreeData.h"
#include "Blueprint/WidgetTree.h" // WidgetTree�� ����ϱ� ���� ����
#include "Components/SkillTreeComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // AttributeSet ��� ����
#include "Kismet/GameplayStatics.h"
#include "Character/GASCharacterPlayer.h"
void USkillTreeNodeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateNodeVisuals();
}

void USkillTreeNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateNodeState(Cast<USkillTreeComponent>(Cast<AGASCharacterPlayer>(GetOwningPlayerPawn())->GetSkillTreeComponent()));
	
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
	if (NodeBorder)				//���⼭ Crash
	{
		NodeBorder->SetBrushColor(bIsSelected ? SelectedColor : DefaultColor);
	}
}

void USkillTreeNodeWidget::UpdateNodeState(USkillTreeComponent* SkillTreeComp)
{
	if (!SkillTreeComp || !NodeBorder || !SkillCostText) return;

	// ������ ���̺� �ڵ鿡�� ��ų �����͸� �����ɴϴ�.
	if (!CachedSkillData && SkillRowHandle.DataTable && SkillRowHandle.RowName != NAME_None)
	{
		CachedSkillData = SkillRowHandle.GetRow<FSkillTreeDataRow>(TEXT(""));
	}
	if (!CachedSkillData) return;

	// 1. ���� ��ų ���� Ȯ��
	bool bPrerequisiteMet = true;
	if (CachedSkillData->PrerequisiteSkillID != NAME_None)
	{
		bPrerequisiteMet = SkillTreeComp->HasLearnedSkill(CachedSkillData->PrerequisiteSkillID);
	}

	// ���� ��ų�� ����� ���ߴٸ� ��带 ��� ���·� ǥ��
	if (!bPrerequisiteMet)
	{
		//NodeBorder->SetBrushColor(LockedColor);
		NodeBorder->SetContentColorAndOpacity(LockedColor);
		//SetIsEnabled(false); // ��� ��Ȱ��ȭ
		return;
	}

	// ���� ��ų�� ����ٸ� Ȱ��ȭ�ϰ� �⺻ �������� ����
	//SetIsEnabled(true);
	NodeBorder->SetContentColorAndOpacity(UnlockedColor);

	//NodeBorder->SetBrushColor(UnlockedColor);

	//
		// 2. ��ų ����Ʈ ���� Ȯ��
	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FGameplayAttribute SkillPointAttribute = UPlayerCharacterAttributeSet::GetSkillPointAttribute();

	if (SkillPointAttribute.IsValid())
	{
		int32 PlayerSkillPointCost = Player->GetAbilitySystemComponent()->GetNumericAttribute(SkillPointAttribute);
		if (PlayerSkillPointCost < CachedSkillData->SkillPointCost)
		{
			// ��ų ����Ʈ�� �����ϸ� �ؽ�Ʈ�� ����������
			SkillCostText->SetColorAndOpacity(InsufficientPointsColor);
		}
		else
		{
			// ����ϸ� �⺻ ��������
			SkillCostText->SetColorAndOpacity(DefaultCostColor);
		}
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
		UE_LOG(LogTemp, Warning, TEXT("SkillIcon is valid for skill %s"), *SkillData.SkillID.ToString());
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

}
void USkillTreeNodeWidget::UpdateNodeVisuals()
{

	if (!SkillRowHandle.DataTable || SkillRowHandle.RowName == NAME_None)
	{
		CachedSkillData = nullptr;
		return;
	}
	// ������ ���̺��� RowName�� �ش��ϴ� �����͸� ã�� �ε��մϴ�.
	const FString ContextString(TEXT("USkillNodeWidget::UpdateNodeVisuals"));
	CachedSkillData = SkillRowHandle.GetRow<FSkillTreeDataRow>(ContextString);
	SkillData = *CachedSkillData; // ĳ�õ� �����͸� ���� SkillData�� ����
	SetSkillData(*CachedSkillData); // UI �������� ������Ʈ�մϴ�.

}




