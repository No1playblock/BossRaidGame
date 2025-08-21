// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attribute/CharacterAttributeSet.h"
#include "MobCharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UMobCharacterAttributeSet : public UCharacterAttributeSet
{
	GENERATED_BODY()
	
public:
	UMobCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UMobCharacterAttributeSet, ExpReward);

	void Reset();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Reward", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ExpReward;
};
