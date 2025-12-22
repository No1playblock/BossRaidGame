// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UInventoryComponent;
UCLASS()
class BOSSRAIDGAME_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 외부에서 데이터를 넣어주는 함수
    void UpdateSlot(FName InItemID, int32 InQuantity, int32 InSlotIndex, UInventoryComponent* InOwnerComp);

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> IconImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> QuantityText;

private:
    FName ItemID;
    int32 Quantity;
	int32 SlotIndex;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> OwnerComp;
};
