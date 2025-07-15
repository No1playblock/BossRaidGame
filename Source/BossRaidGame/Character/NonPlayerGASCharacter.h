// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "NonPlayerGASCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API ANonPlayerGASCharacter : public ABaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ANonPlayerGASCharacter();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnOutOfHealth() override;

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UMobCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere)
	float ExpReward = 0.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> GainExpEffectClass;

	UPROPERTY(EditAnywhere, Category = Pawn)
	TObjectPtr<class UBehaviorTree> BehaviorTree;
	
};
