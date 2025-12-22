// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UQuickSlotComponent;
class UInventoryComponent;

UCLASS()
class BOSSRAIDGAME_API UQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 슬롯 초기화 및 업데이트
	void UpdateQuickSlot(int32 InSlotIndex, UQuickSlotComponent* InQuickSlotComp, UInventoryComponent* InInventoryComp);

protected:
	// 드롭 받았을 때 (아이템 등록 로직)
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void RefreshUI();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	// 수량 표시 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HotKeyText;

	UPROPERTY(EditAnywhere)
	int32 SlotIndex;
private:
	

	UPROPERTY()
	TObjectPtr<UQuickSlotComponent> QuickSlotComp;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;
};
