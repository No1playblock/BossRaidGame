// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeNodeWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameData/SkillTreeData.h"
#include "Blueprint/WidgetTree.h" // WidgetTree를 사용하기 위해 포함
#include "Components/SkillTreeComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // AttributeSet 헤더 포함
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
	if (NodeBorder)				//여기서 Crash
	{
		NodeBorder->SetBrushColor(bIsSelected ? SelectedColor : DefaultColor);
	}
}

void USkillTreeNodeWidget::UpdateNodeState(USkillTreeComponent* SkillTreeComp)
{
	if (!SkillTreeComp || !NodeBorder || !SkillCostText) return;

	// 데이터 테이블 핸들에서 스킬 데이터 가져옴
	if (!CachedSkillData && SkillRowHandle.DataTable && SkillRowHandle.RowName != NAME_None)
	{
		CachedSkillData = SkillRowHandle.GetRow<FSkillTreeDataRow>(TEXT(""));
	}
	if (!CachedSkillData) return;

	//선행 스킬 조건 확인
	bool bPrerequisiteMet = true;
	if (CachedSkillData->PrerequisiteSkillID != NAME_None)
	{
		bPrerequisiteMet = SkillTreeComp->HasLearnedSkill(CachedSkillData->PrerequisiteSkillID);
	}

	// 선행 스킬을 배우지 못했다면 노드를 잠김 상태로 표시
	if (!bPrerequisiteMet)
	{
		//NodeBorder->SetBrushColor(LockedColor);
		NodeBorder->SetContentColorAndOpacity(LockedColor);
		//SetIsEnabled(false); // 노드 비활성화
		return;
	}

	// 선행 스킬을 배웠다면 활성화하고 기본 색상으로 변경
	//SetIsEnabled(true);
	NodeBorder->SetContentColorAndOpacity(UnlockedColor);

		//스킬 포인트 조건 확인
	AGASCharacterPlayer* Player = Cast<AGASCharacterPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FGameplayAttribute SkillPointAttribute = UPlayerCharacterAttributeSet::GetSkillPointAttribute();

	if (SkillPointAttribute.IsValid())
	{
		int32 PlayerSkillPointCost = Player->GetAbilitySystemComponent()->GetNumericAttribute(SkillPointAttribute);
		if (PlayerSkillPointCost < CachedSkillData->SkillPointCost)
		{
			// 스킬 포인트가 부족하면 텍스트를 빨간색으로
			SkillCostText->SetColorAndOpacity(InsufficientPointsColor);
		}
		else
		{
			// 충분하면 기본 색상으로
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
	// 데이터 테이블에서 RowName에 해당하는 데이터를 찾아 로드
	const FString ContextString(TEXT("USkillNodeWidget::UpdateNodeVisuals"));
	CachedSkillData = SkillRowHandle.GetRow<FSkillTreeDataRow>(ContextString);
	SkillData = *CachedSkillData; 
	SetSkillData(*CachedSkillData); // UI 위젯들을 업데이트

}




