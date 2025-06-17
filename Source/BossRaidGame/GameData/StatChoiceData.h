#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h" // FGameplayAttribute
#include "StatChoiceData.generated.h"


UENUM(BlueprintType)
enum class EStatChoiceType : uint8
{
	AttackPower,
	AttackSpeed,
	SkillPower,
	SkillCooldownRate,
	MoveSpeed
};

USTRUCT(BlueprintType)
struct FStatChoiceInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayLevelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute TargetLevelAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ModifierValue;

	FStatChoiceInfo() : ModifierValue(0.f) {}

	FStatChoiceInfo(const FText& InDisplayName, const FText& InDisplayLevelText, float InValue, FGameplayAttribute InAttribute, const FGameplayAttribute InTargetLevelAttribute)
		: DisplayName(InDisplayName)
		, ModifierValue(InValue)
		,DisplayLevelText(InDisplayLevelText)
		, TargetAttribute(InAttribute) 
		, TargetLevelAttribute(InTargetLevelAttribute)
	{
		FString ValueString = FString::Printf(TEXT("%+.1f"), InValue);
		DisplayValue = FText::FromString(ValueString);
	}
};