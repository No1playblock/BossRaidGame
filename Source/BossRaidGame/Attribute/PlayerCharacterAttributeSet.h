// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attribute/CharacterAttributeSet.h"
#include "PlayerCharacterAttributeSet.generated.h"

/**
 *
 */
UCLASS()
class BOSSRAIDGAME_API UPlayerCharacterAttributeSet : public UCharacterAttributeSet
{
	GENERATED_BODY()

public:
	UPlayerCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, CurrentExp);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, CurrentLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, AttackPowerLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, AttackSpeedLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, SkillPowerLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, SkillCooldownRateLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, MoveSpeedLevel);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, SkillPower);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, SkillCooldownRate);
	ATTRIBUTE_ACCESSORS(UPlayerCharacterAttributeSet, SkillPoint);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	float GetTotalExpForLevel(int32 Level) const;

protected:


	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentExp;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPowerLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeedLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillPowerLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillCooldownRateLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeedLevel;

	UPROPERTY(BlueprintReadOnly, Category = "Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillPower;

	UPROPERTY(BlueprintReadOnly, Category = "Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillCooldownRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	FGameplayAttributeData SkillPoint;

	TObjectPtr<class UCurveTable> LevelTable;



};
