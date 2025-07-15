// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/SkillTreeData.h"
#include "SkillInfoWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillInfoClosed); // ��ų ���� UI ���� �̺�Ʈ
/**
 * 
 */
class UImage;
class UTextBlock;

UCLASS()
class BOSSRAIDGAME_API USkillInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void OpenSkillInfo(const FSkillTreeDataRow& SkillData);

	UPROPERTY(BlueprintAssignable)
	FOnSkillInfoClosed OnSkillInfoClosed; // ��ų ���� UI ���� �̺�Ʈ
protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	UFUNCTION()
	void CloseUI();

	UFUNCTION()
	void OnBuyButtonClicked();

	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoolTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillCostText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BuyButton;

	FName SkillID;


	int SkillPointCost = 0; // ��ų ����Ʈ ����� ������ ����
};
