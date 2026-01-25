// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDamageText(float DamageAmount);

protected:
	virtual void NativeConstruct() override;

	// 애니메이션이 끝나면 호출될 델리게이트 함수
	UFUNCTION()
	void OnAnimFinished();
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FloatAnim;
};
