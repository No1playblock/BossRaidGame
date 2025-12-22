// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMainWidget.generated.h"

/**
 * 
 */
class UWrapBox;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class BOSSRAIDGAME_API UInventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 인벤토리 컴포넌트와 연결하고 초기화
    UFUNCTION()
    void InitInventory(UInventoryComponent* InInventoryComp);

protected:
    // UI 갱신 함수 (델리게이트에 바인딩됨)
    UFUNCTION()
    void RefreshInventory();

protected:
    // 슬롯들을 담을 컨테이너 (이름 일치 필수)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> Grid_Slots;

    // 생성할 슬롯 위젯 클래스 (에디터에서 WBP_InventorySlot 할당)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UInventoryComponent> InventoryComp;
};
