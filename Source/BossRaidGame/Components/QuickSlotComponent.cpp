// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuickSlotComponent.h"
#include "Components/InventoryComponent.h"
#include "GameData/ItemData.h" // FItemData
#include "AbilitySystemComponent.h" // GAS 필수
#include "AbilitySystemInterface.h" // GAS 인터페이스
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"

UQuickSlotComponent::UQuickSlotComponent()
{
    QuickSlotItems.Init(NAME_None, MaxQuickSlots);
}

// Called when the game starts
void UQuickSlotComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UQuickSlotComponent::RegisterItemToSlot(int32 SlotIndex, FName ItemID)
{
	if (QuickSlotItems.IsValidIndex(SlotIndex))
	{
		QuickSlotItems[SlotIndex] = ItemID;

		// UI 갱신 알림
		if (OnQuickSlotUpdated.IsBound())
		{
			OnQuickSlotUpdated.Broadcast();
		}
	}
}

FName UQuickSlotComponent::GetItemInSlot(int32 SlotIndex) const
{
	if (QuickSlotItems.IsValidIndex(SlotIndex))
	{
		return QuickSlotItems[SlotIndex];
	}
	return NAME_None;
}

void UQuickSlotComponent::ActivateSlot(int32 SlotIndex)
{
	if (!QuickSlotItems.IsValidIndex(SlotIndex)) return;

	FName ItemID = QuickSlotItems[SlotIndex];
	if (ItemID.IsNone()) return;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UInventoryComponent* Inventory = OwnerActor->FindComponentByClass<UInventoryComponent>();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);

	if (!Inventory || !ASC) return;

	// 아이템 데이터 확인
	const FItemData* ItemInfo = Inventory->GetItemData(ItemID);
	if (!ItemInfo || !ItemInfo->UseAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickSlot: No Ability found for Item %s"), *ItemID.ToString());
		return;
	}

	//아이템 타입에 따른 처리
	if (ItemInfo->ItemType == EItemType::Consumable)
	{
		// 소모품은 수량이 있어야 사용 가능
		int32 CurrentQty = Inventory->GetTotalQuantity(ItemID);
		if (CurrentQty > 0)
		{
			// 어빌리티 클래스 유효성 확인
			if (!ItemInfo->UseAbilityClass) return;

			//어빌리티가 등록되어 있는지 확인
			FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(ItemInfo->UseAbilityClass);

			if (!Spec)
			{
				// 등록 안 되어 있으면 등록
				FGameplayAbilitySpec NewSpec(ItemInfo->UseAbilityClass, 1, -1, GetOwner());
				ASC->GiveAbility(NewSpec);

			}

			// 아이템 소모
			Inventory->RemoveItem(ItemID, 1);

			//실행 결과 확인용 bool 변수
			bool bSuccess = ASC->TryActivateAbilityByClass(ItemInfo->UseAbilityClass, true);

			if (bSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Ability Activated Successfully!"));
			}
			else
			{
				// 태그 문제, 코스트 부족, 쿨타임 등으로 실패할 수 있음
				UE_LOG(LogTemp, Error, TEXT("Failed to Activate Ability (Registered but blocked)"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Not enough items!"));
		}
	}
	else if (ItemInfo->ItemType == EItemType::Equipment)
	{
		// 장비는 소모하지 않고 장착 어빌리티 발동
		ASC->TryActivateAbilityByClass(ItemInfo->UseAbilityClass, true);
	}
}
