// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "GameData/ItemData.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
void UInventoryComponent::SwapSlot(int32 SourceIndex, int32 DestIndex)
{
    if (!InventorySlots.IsValidIndex(SourceIndex) || !InventorySlots.IsValidIndex(DestIndex)) return;
    if (SourceIndex == DestIndex) return;

    // 언리얼의 Swap 함수로 간단하게 배열 요소 교체
    InventorySlots.Swap(SourceIndex, DestIndex);

    // UI 업데이트
    if (OnInventoryUpdated.IsBound())
    {
        OnInventoryUpdated.Broadcast();
    }
}
int32 UInventoryComponent::GetTotalQuantity(FName ItemID) const
{
    if (ItemID.IsNone()) return 0;

    int32 TotalCount = 0;
    for (const FInventorySlot& Slot : InventorySlots)
    {
        if (Slot.ItemID == ItemID)
        {
            TotalCount += Slot.Quantity;
        }
    }
    return TotalCount;
}
void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // 인벤토리 슬롯 초기화 (빈 슬롯으로 채움)
    InventorySlots.Init(FInventorySlot(), MaxSlots);

    AddItem(FName("Potion_HP_Small"), 5);
}

const FItemData* UInventoryComponent::GetItemData(FName ItemID) const
{
    if (!ItemDataTable || ItemID.IsNone()) return nullptr;

    // 데이터 테이블에서 RowName(ItemID)으로 정보 찾기
    return ItemDataTable->FindRow<FItemData>(ItemID, TEXT("InventoryComponent"));
}

int32 UInventoryComponent::AddItem(FName ItemID, int32 Amount)
{
    if (!ItemDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT(" [AddItem Failed] ItemDataTable is NULL! Check Blueprint Details."));
        return Amount;
    }
    if (ItemID.IsNone() || Amount <= 0) return Amount;

    if (!ItemDataTable || ItemID.IsNone() || Amount <= 0) return Amount;

    // 해당 아이템의 최대 중첩 수(MaxStack) 확인
    const FItemData* ItemInfo = GetItemData(ItemID);
    const int32 MaxStack = ItemInfo ? ItemInfo->MaxStack : 99;


    if (!ItemInfo)
    {
        UE_LOG(LogTemp, Error, TEXT("[AddItem Failed] Could not find ItemID: '%s' in DataTable."), *ItemID.ToString());
        return Amount;
    }


    // 이미 가지고 있는 슬롯에 먼저 쌓기 (Stacking)
    for (FInventorySlot& Slot : InventorySlots)
    {
        if (Slot.ItemID == ItemID && Slot.Quantity < MaxStack)
        {
            int32 SpaceLeft = MaxStack - Slot.Quantity; // 남은 공간
            int32 AddAmount = FMath::Min(SpaceLeft, Amount); // 실제로 넣을 양

            Slot.Quantity += AddAmount;
            Amount -= AddAmount;

            if (Amount <= 0) break; // 다 넣었으면 종료
        }
    }

    // 남은 수량이 있다면 빈 슬롯에 넣기
    if (Amount > 0)
    {
        for (FInventorySlot& Slot : InventorySlots)
        {
            if (Slot.ItemID.IsNone()) // 빈 슬롯 발견
            {
                int32 AddAmount = FMath::Min(MaxStack, Amount);

                Slot.ItemID = ItemID;
                Slot.Quantity = AddAmount;

                Amount -= AddAmount;

                if (Amount <= 0) break; // 다 넣었으면 종료
            }
        }
    }

    // UI 업데이트 알림
    if (OnInventoryUpdated.IsBound())
    {
        OnInventoryUpdated.Broadcast();
    }

    // 남은 수량 반환 (0이면 성공, 0보다 크면 인벤토리 꽉 참)
    return Amount;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Amount)
{
    if (Amount <= 0) return false;

    int32 AmountToRemove = Amount;

    // 뒤에서부터 검색하거나 앞에서부터 검색 (여기선 앞에서부터)
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].ItemID == ItemID)
        {
            if (InventorySlots[i].Quantity > AmountToRemove)
            {
                // 수량이 충분하면 차감하고 종료
                InventorySlots[i].Quantity -= AmountToRemove;
                AmountToRemove = 0;
                break;
            }
            else
            {
                // 수량이 부족하면 해당 슬롯을 비우고, 남은 양만큼 다음 슬롯 찾기
                AmountToRemove -= InventorySlots[i].Quantity;
                InventorySlots[i] = FInventorySlot(); // 슬롯 초기화 (빈 칸)
            }
        }
    }

    // UI 업데이트
    if (OnInventoryUpdated.IsBound())
    {
        OnInventoryUpdated.Broadcast();
    }

    return (AmountToRemove == 0); // 모두 제거했으면 true
}

FInventorySlot UInventoryComponent::GetSlotInfo(int32 SlotIndex) const
{
    if (InventorySlots.IsValidIndex(SlotIndex))
    {
        return InventorySlots[SlotIndex];
    }
    return FInventorySlot();
}


