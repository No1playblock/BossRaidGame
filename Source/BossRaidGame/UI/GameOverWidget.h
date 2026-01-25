// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnYesBtnClicked();

	UFUNCTION()
	void OnNoBtnClicked();


protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> YesBtn;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> NoBtn;


};
