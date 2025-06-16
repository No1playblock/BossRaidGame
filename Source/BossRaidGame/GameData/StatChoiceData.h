#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h" // FGameplayAttribute
#include "StatChoiceData.generated.h"

USTRUCT(BlueprintType)
struct FStatChoiceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ModifierValue;

	FStatChoiceInfo() : ModifierValue(0.f) {}

	FStatChoiceInfo(const FText& InDisplayName, float InValue, FGameplayAttribute InAttribute)
		: DisplayName(InDisplayName)
		, TargetAttribute(InAttribute) 
		, ModifierValue(InValue)
	{
		FString ValueString = FString::Printf(TEXT("%+.1f"), InValue);
		DisplayValue = FText::FromString(ValueString);
	}
};