// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ItemData.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID; // 데이터 테이블의 RowName

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity; // 수량

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SlotIndex;
	// 생성자: 기본값은 빈 슬롯(None, 0)
	FInventorySlot() : ItemID(NAME_None), Quantity(0) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSRAIDGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

    const TArray<FInventorySlot>& GetInventorySlots() const { return InventorySlots; }

    void SwapSlot(int32 SourceIndex, int32 DestIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetTotalQuantity(FName ItemID) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
    // 아이템 추가 (성공 시 남은 수량 반환, 0이면 모두 성공)
    int32 AddItem(FName ItemID, int32 Amount);

    // 아이템 제거
    bool RemoveItem(FName ItemID, int32 Amount);

    // 특정 슬롯 정보 가져오기
    FInventorySlot GetSlotInfo(int32 SlotIndex) const;

    // 데이터 테이블 정보 가져오기
    const FItemData* GetItemData(FName ItemID) const;

    // UI 업데이트 알림용 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

protected:
    // 실제 아이템이 저장될 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> InventorySlots;

    // 인벤토리 최대 칸 수
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int32 MaxSlots = 20;

    // 참조할 아이템 데이터 테이블
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TObjectPtr<UDataTable> ItemDataTable;
		
};
