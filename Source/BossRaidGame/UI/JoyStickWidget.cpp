// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/JoyStickWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UJoyStickWidget::HandleTouchPress(const FVector2D& InTouchLocation)
{
	TouchStartLocation = InTouchLocation;
	CurrentMoveVector = FVector2D::ZeroVector;

	if (JoystickBase && JoystickThumb)
	{
		JoystickBase->SetVisibility(ESlateVisibility::Visible);
		JoystickThumb->SetVisibility(ESlateVisibility::Visible);

		//UE_LOG(LogTemp, Warning, TEXT("Touch Pressed at: %s"), *InTouchLocation.ToString());

		// DPI ����
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		FVector2D LocalPos = InTouchLocation / ViewportScale;

		UE_LOG(LogTemp, Warning, TEXT("Touch:%s  Local:%s  Scale:%f"),
			*InTouchLocation.ToString(),
			*LocalPos.ToString(),
			ViewportScale);

		if (UCanvasPanelSlot* BaseSlot = Cast<UCanvasPanelSlot>(JoystickBase->Slot))
		{
			BaseSlot->SetPosition(LocalPos - BaseSlot->GetSize() * 0.5f);
		}

		if (UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(JoystickThumb->Slot))
		{
			ThumbSlot->SetPosition(LocalPos - ThumbSlot->GetSize() * 0.5f);
		}
	}
}

void UJoyStickWidget::HandleTouchMove(const FVector2D& InTouchLocation)
{
	float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

	// DPI ������ ��ǥ
	FVector2D LocalTouchStart = TouchStartLocation / Scale;
	FVector2D LocalTouchPos = InTouchLocation / Scale;

	UE_LOG(LogTemp, Warning, TEXT("Touch Moved to: %s (Local: %s)"),
		*InTouchLocation.ToString(), *LocalTouchPos.ToString());

	// �̵��� ���
	FVector2D Delta = LocalTouchPos - LocalTouchStart;

	const float MaxRadius = 100.f;
	if (Delta.Size() > MaxRadius)
	{
		Delta = Delta.GetSafeNormal() * MaxRadius;
	}

	CurrentMoveVector = FVector2D(Delta.X, -Delta.Y) / MaxRadius;


	if (JoystickThumb)
	{
		if (UCanvasPanelSlot* ThumbSlot = Cast<UCanvasPanelSlot>(JoystickThumb->Slot))
		{
			ThumbSlot->SetPosition(LocalTouchStart + Delta - ThumbSlot->GetSize() * 0.5f);
		}
	}
}

void UJoyStickWidget::HandleTouchRelease()
{
	CurrentMoveVector = FVector2D::ZeroVector;

	if (JoystickBase && JoystickThumb)
	{
		JoystickBase->SetVisibility(ESlateVisibility::Hidden);
		JoystickThumb->SetVisibility(ESlateVisibility::Hidden);
	}
}
