// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoyStickWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UJoyStickWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void HandleTouchPress(const FVector2D& InTouchLocation);
	void HandleTouchMove(const FVector2D& InTouchLocation);
	void HandleTouchRelease();

	// ���� �̵� ���͸� PlayerController�� �����ϱ� ���� Getter
	FVector2D GetMoveVector() const { return CurrentMoveVector; }
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoystickBase;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> JoystickThumb;


private:
	// ���̽�ƽ�� ���� ���¸� ������ ������
	FVector2D TouchStartLocation;
	FVector2D CurrentMoveVector;

};
