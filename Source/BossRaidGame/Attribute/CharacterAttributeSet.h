// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "CharacterAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE(FOutOfHealthDelegate);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthDelegate);
/**
 * 
 */



UCLASS()
class BOSSRAIDGAME_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, AttackPower);
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MoveSpeed);
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Damage);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthDelegate OnOutOfHealth;


protected:
	

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Utility", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Damage", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADamageIndicatorActor> DamageIndicatorClass;

	bool bOutOfHealth = false;
};
