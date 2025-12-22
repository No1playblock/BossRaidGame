// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "BRDragDropOperation.generated.h"

UENUM(BlueprintType)
enum class EDragType : uint8
{
    None,
    Item,       // 인벤토리 아이템
    Skill,      // 스킬
    QuickSlot   // 퀵슬롯 변경
};
/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UBRDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    EDragType DragType;

    // (ItemID 혹은 SkillID)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    FName DataID;

    //(아이템일 경우) 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    int32 Quantity;

    // 범용성을 위해 UObject*로 저장, 나중에 Cast해서 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    UObject* SourceObject;

    //몇 번째 칸에서 왔는가? (인벤토리 스왑용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop")
    int32 SourceIndex;
};
