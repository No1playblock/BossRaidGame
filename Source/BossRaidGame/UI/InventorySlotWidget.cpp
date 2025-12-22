// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/InventoryComponent.h"
#include "GameData/ItemData.h"
#include "Operation/BRDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // 드래그 감지용 라이브러리
void UInventorySlotWidget::UpdateSlot(FName InItemID, int32 InQuantity, int32 InSlotIndex, UInventoryComponent* InOwnerComp)
{
    ItemID = InItemID;
    Quantity = InQuantity;
	SlotIndex = InSlotIndex;
    OwnerComp = InOwnerComp;

    //빈 슬롯일때
    if (ItemID.IsNone() || Quantity <= 0)
    {
        if (IconImage) IconImage->SetVisibility(ESlateVisibility::Hidden);
        if (QuantityText) QuantityText->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (OwnerComp)
    {
        // 내부 함수로 데이터 포인터 가져옴
        const FItemData* Info = OwnerComp->GetItemData(ItemID);
        if (Info)
        {
            // 아이콘 설정 (Soft Object Ptr 로드)
            UTexture2D* IconTexture = Info->ItemIcon.LoadSynchronous();

            if (IconImage && IconTexture)
            {
                IconImage->SetBrushFromTexture(IconTexture);
                IconImage->SetVisibility(ESlateVisibility::Visible);
            }

            // 수량 표시 (1개면 숨김, 2개 이상만 표시)
            if (QuantityText)
            {
                if (Quantity > 1)
                {
                    QuantityText->SetText(FText::AsNumber(Quantity));
                    QuantityText->SetVisibility(ESlateVisibility::Visible);
                }
                else
                {
                    // 1개일 때는 숫자 없이 아이콘만 보이게
                    QuantityText->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }
    }
}
FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    // 좌클릭이고, 유효한 스킬 데이터가 있을 때
    if (OwnerComp && InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {

        return FReply::Handled().DetectDrag(this->TakeWidget(), InMouseEvent.GetEffectingButton());
    }

    return FReply::Unhandled();
}
void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    UBRDragDropOperation* DragOp = NewObject<UBRDragDropOperation>();

    //데이터 채우기
    DragOp->DragType = EDragType::Item;          // 타입 지정
    DragOp->DataID = ItemID;                     // 아이템 ID
    DragOp->Quantity = Quantity;                 // 수량
    DragOp->SourceIndex = SlotIndex;             // 출발 슬롯 번호
    DragOp->SourceObject = OwnerComp;            // 출발지 컴포넌트 (InventoryComponent)

    //비주얼 설정
    DragOp->DefaultDragVisual = this;
    DragOp->Pivot = EDragPivot::CenterCenter;

	UE_LOG(LogTemp, Warning, TEXT("UInventorySlotWidget::NativeOnDragDetected - Dragging ItemID: %s from SlotIndex: %d"), *ItemID.ToString(), SlotIndex);
    OutOperation = DragOp;
}
bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

    UBRDragDropOperation* DragOp = NewObject<UBRDragDropOperation>();

    // 유효성검사
    if (DragOp && DragOp->DragType == EDragType::Item)
    {
        // 3. 같은 인벤토리 창 내부에서의 이동인지 확인 (출처 컴포넌트 비교)
        if (DragOp->SourceObject == OwnerComp)
        {
           
            if (OwnerComp)
            {
                OwnerComp->SwapSlot(DragOp->SourceIndex, SlotIndex);
                return true; // 드롭 처리 완료
            }
        }
        else
        {
            // TODO: 창고나 다른 사람 인벤토리에서 온 경우 (추후 구현)
        }
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
