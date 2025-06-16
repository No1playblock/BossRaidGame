// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/StatChoiceData.h" // FStatChoiceInfo
#include "StatChoiceCardWidget.generated.h"

// �������Ʈ���� ���ε��� �� �ִ� ���̳��� ��Ƽĳ��Ʈ ��������Ʈ ����
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

private:
	UFUNCTION()
	void OnSelectButtonClicked();

private:
	UPROPERTY()
	FStatChoiceInfo CurrentChoiceInfo;
};