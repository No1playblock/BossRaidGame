// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gamedata/SkillTreeData.h"
#include "SkillSimpleBuyWidget.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnSimpleBuyClosed);


UCLASS()
class BOSSRAIDGAME_API USkillSimpleBuyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSkillCostText(const FSkillTreeDataRow& SkillData);


public:
	FOnSimpleBuyClosed OnSimpleBuyClosed; // ½ºÅ³ Á¤º¸ UI ´ÝÈû ÀÌº¥Æ®
protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	UFUNCTION()
	void OnBuyButtonClicked();

	UFUNCTION()
	void OnNoButtonClicked();

	

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BuyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkillPointText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> NoButton;

	FName SkillID;
};
