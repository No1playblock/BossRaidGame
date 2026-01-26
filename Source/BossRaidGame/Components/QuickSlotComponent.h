// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickSlotComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSRAIDGAME_API UQuickSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuickSlotComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 퀵슬롯에 아이템 등록 (UI 드롭 시 호출)
	UFUNCTION()
	void RegisterItemToSlot(int32 SlotIndex, FName ItemID);

	// 슬롯에 등록된 아이템 ID 가져오기
	UFUNCTION()
	FName GetItemInSlot(int32 SlotIndex) const;

	// 퀵슬롯 사용 (키보드 입력 시 호출 -> 나중에 구현)
	UFUNCTION()
	void ActivateSlot(int32 SlotIndex);

	FOnQuickSlotUpdated OnQuickSlotUpdated;

private:
	// 슬롯 개수 (예: 1~4번 키)
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxQuickSlots = 4;

	// 슬롯 데이터 배열 (저장되는 건 ItemID 뿐)
	UPROPERTY()
	TArray<FName> QuickSlotItems;
		
};
