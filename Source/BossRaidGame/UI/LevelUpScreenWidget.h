// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/StatChoiceData.h"
#include "LevelUpScreenWidget.generated.h"

/**
 * 
 */
class UHorizontalBox;
class UStatChoiceCardWidget;
class AGASCharacterPlayer;

UCLASS()
class BOSSRAIDGAME_API ULevelUpScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void InitializeChoices(const TArray<FStatChoiceInfo>& Choices, AGASCharacterPlayer* InPlayerOwner);

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> ChoiceContainer;

	UPROPERTY(EditDefaultsOnly, Category = "LevelUp")
	TSubclassOf<UStatChoiceCardWidget> ChoiceCardClass;

private:

	UFUNCTION()
	void OnCardSelected(const FStatChoiceInfo& SelectedChoice);

private:
	UPROPERTY()
	TObjectPtr<AGASCharacterPlayer> PlayerOwner;
};
