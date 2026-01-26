// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryMainWidget.h"
#include "Components/WrapBox.h"
#include "InventorySlotWidget.h"
#include "Components/InventoryComponent.h"

void UInventoryMainWidget::InitInventory(UInventoryComponent* InInventoryComp)
{
    if (!InInventoryComp) return;

    InventoryComp = InInventoryComp;

    // 기존 바인딩 제거 후 다시 연결
    InventoryComp->OnInventoryUpdated.RemoveAll(this);
    InventoryComp->OnInventoryUpdated.AddUObject(this, &UInventoryMainWidget::RefreshInventory);

    // 최초 1회 갱신
    RefreshInventory();
}

void UInventoryMainWidget::RefreshInventory()
{
    if (!InventoryComp || !Grid_Slots || !SlotWidgetClass) return;

    // 기존 슬롯 싹 지우기
    Grid_Slots->ClearChildren();

    // 인벤토리 데이터를 순회하며 슬롯 생성
    const TArray<FInventorySlot>& Slots = InventoryComp->GetInventorySlots();

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        const FInventorySlot& SlotData = Slots[i];

        UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (NewSlot)
        {
            NewSlot->UpdateSlot(SlotData.ItemID, SlotData.Quantity, i, InventoryComp);
            Grid_Slots->AddChild(NewSlot);
        }
    }
}