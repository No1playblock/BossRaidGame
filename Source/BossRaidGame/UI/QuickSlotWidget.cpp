// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QuickSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/QuickSlotComponent.h"
#include "Components/InventoryComponent.h"
#include "GameData/ItemData.h"
#include "Operation/BRDragDropOperation.h"


void UQuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//핫키 텍스트 설정 (0번 인덱스 -> 1번 키)
	if (HotKeyText)
	{
		HotKeyText->SetText(FText::AsNumber(SlotIndex + 1));
	}

	// 소유하고 있는 폰(캐릭터) 찾기
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (OwningPawn)
	{
		//컴포넌트 가져오기
		QuickSlotComp = OwningPawn->FindComponentByClass<UQuickSlotComponent>();
		InventoryComp = OwningPawn->FindComponentByClass<UInventoryComponent>();

		// 델리게이트 연결 (데이터 변경 감지)
		if (QuickSlotComp)
		{
			// 안전하게 기존 연결 해제 후 다시 연결
			QuickSlotComp->OnQuickSlotUpdated.RemoveAll(this);
			QuickSlotComp->OnQuickSlotUpdated.AddUObject(this, &UQuickSlotWidget::RefreshUI);
		}

		//인벤토리가 바뀌면 QuickSlot도 갱신
		if (InventoryComp)
		{
			InventoryComp->OnInventoryUpdated.RemoveAll(this);
			InventoryComp->OnInventoryUpdated.AddUObject(this, &UQuickSlotWidget::RefreshUI);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[QuickSlot] Could not find Owning Pawn!"));
		return;
	}

	// 최초 갱신
	RefreshUI();
}

void UQuickSlotWidget::RefreshUI()
{
	// 컴포넌트가 없으면 아무것도 못함
	if (!QuickSlotComp || !InventoryComp) return;

	// 등록된 ItemID 가져오기
	FName ItemID = QuickSlotComp->GetItemInSlot(SlotIndex);

	if (ItemID.IsNone())
	{
		if (IconImage) IconImage->SetVisibility(ESlateVisibility::Hidden);
		if (QuantityText) QuantityText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	// 아이템 정보 로드 및 표시
	const FItemData* Info = InventoryComp->GetItemData(ItemID);
	if (Info)
	{
		UTexture2D* IconTexture = Info->ItemIcon.LoadSynchronous();
		if (IconImage && IconTexture)
		{
			IconImage->SetBrushFromTexture(IconTexture);
			IconImage->SetVisibility(ESlateVisibility::Visible);
			IconImage->SetColorAndOpacity(FLinearColor::White);
		}
	}
	// 수량 표시
	if (QuantityText)
	{
		int32 TotalCount = InventoryComp->GetTotalQuantity(ItemID);

		if (TotalCount > 0)
		{
			QuantityText->SetText(FText::AsNumber(TotalCount));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			QuantityText->SetText(FText::FromString("0"));
			QuantityText->SetVisibility(ESlateVisibility::Visible);

			// 수량 없으면 어둡게
			if (IconImage) IconImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		}
	}
}
bool UQuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//캐스트
	UBRDragDropOperation* BRDragOperation = Cast<UBRDragDropOperation>(InOperation);

	// 아이템인지 확인
	if (BRDragOperation && BRDragOperation->DragType == EDragType::Item)
	{
		//퀵슬롯 컴포넌트에 요청
		if (QuickSlotComp)
		{
			QuickSlotComp->RegisterItemToSlot(SlotIndex, BRDragOperation->DataID);
			RefreshUI();
			return true; // 성공
		}
	}

	return true;
}

