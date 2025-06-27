// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillTreeNodeWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "GameData/SkillTreeData.h"

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

}
void USkillTreeNodeWidget::UpdateNodeVisuals()
{
	// 핸들이 유효하지 않으면 아무것도 하지 않습니다.
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
	//if (CachedSkillData)
	//{
	//	// 로드한 데이터로 UI 위젯들을 채웁니다.
	//	if (SkillIcon)
	//	{
	//		SkillIcon->SetBrushFromTexture(CachedSkillData->SkillIcon.LoadSynchronous());
	//	}
	//	if (SkillNameText)
	//	{
	//		SkillNameText->SetText(CachedSkillData->SkillName);
	//	}
	//	// ... 나머지 TextBlock들도 위와 같은 방식으로 데이터를 설정 ...
	//}
}
void USkillTreeNodeWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
	UpdateNodeVisuals();
    // 추가 초기화 코드 작성
}

