// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "CharacterPlayer.h"
#include "GameData/StatChoiceData.h"
#include "GASCharacterPlayer.generated.h"


class UDataTable;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API AGASCharacterPlayer : public ACharacterPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:

	AGASCharacterPlayer();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPlayerCharacterAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkillComponent> SkillComponent;

	FDelegateHandle MoveSpeedChangedDelegateHandle;

};
