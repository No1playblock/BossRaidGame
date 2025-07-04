// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gamedata/SkillTreeData.h"
#include "SkillSimpleBuyWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimpleBuyClosed);


UCLASS()
class BOSSRAIDGAME_API USkillSimpleBuyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSkillCostText(const FSkillTreeDataRow& SkillData);


public:
	UPROPERTY(BlueprintAssignable)
	FOnSimpleBuyClosed OnSimpleBuyClosed; // ��ų ���� UI ���� �̺�Ʈ
protected:
	virtual void NativeConstruct() override;

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
