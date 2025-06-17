// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/StatChoiceData.h" // FStatChoiceInfo
#include "StatChoiceCardWidget.generated.h"

// 블루프린트에서 바인딩할 수 있는 다이나믹 멀티캐스트 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceCardSelectedSignature, const FStatChoiceInfo&, ChoiceInfo);

UCLASS()
class BOSSRAIDGAME_API UStatChoiceCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetChoiceInfo(const FStatChoiceInfo& InChoiceInfo);

	UPROPERTY(BlueprintAssignable, Category = "ChoiceCard")
	FOnChoiceCardSelectedSignature OnChoiceCardSelected;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SelectButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StatNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StatValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StatLevelText;
private:
	UFUNCTION()
	void OnSelectButtonClicked();

private:
	UPROPERTY()
	FStatChoiceInfo CurrentChoiceInfo;
};