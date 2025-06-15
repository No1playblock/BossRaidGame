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


	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	void UpdateLevelFromExp(float Exp);

protected:


	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentExp;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Status", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentLevel;

	TObjectPtr<class UCurveTable> LevelTable;
	
};
