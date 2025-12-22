// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

class UGameplayAbility;
class UTexture2D;


// 아이템 대분류
UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable  UMETA(DisplayName = "Consumable"),
    Equipment   UMETA(DisplayName = "Equipment"),
    Etc         UMETA(DisplayName = "etc")
};

//  장비 장착 슬롯 (장비일 경우만 사용)
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    None        UMETA(DisplayName = "None"),
    Weapon      UMETA(DisplayName = "Weapon"),
    Head        UMETA(DisplayName = "Head"),
    Chest       UMETA(DisplayName = "Chest"),
    Feet        UMETA(DisplayName = "Feet")
};


// 소모품 타입 정의
UENUM(BlueprintType)
enum class EConsumableType : uint8
{
    Recovery    UMETA(DisplayName = "Recovery"),
    Buff        UMETA(DisplayName = "Buff"),
    Attack      UMETA(DisplayName = "Attack")
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    TSoftObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    EItemType ItemType; // 소모품, 장비 구분

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    int32 Price = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")
    int32 MaxStack = 1; // 장비는 1, 소모품은 99

    // 소모품 전용 (ItemType == Consumable일 때만 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
    EConsumableType ConsumableType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
    TSubclassOf<UGameplayAbility> UseAbilityClass; // 사용 효과

    //장비 전용 (ItemType == Equipment일 때만 사용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    EEquipmentSlot EquipmentSlot;

    // 장비 스탯 (공격력 또는 방어력) -> GAS Attribute로 적용 예정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    float EquipmentStatValue;

    // 장착 시 캐릭터에 보여줄 메쉬 (소프트 레퍼런스 권장)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    TSoftObjectPtr<USkeletalMesh> EquipmentMesh;

    // 무기의 경우 StaticMesh를 쓸 수도 있음 (필요시 추가)
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    // TSoftObjectPtr<UStaticMesh> WeaponMesh;
};

