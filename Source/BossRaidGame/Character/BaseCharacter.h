// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/ComboActionData.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BOSSRAIDGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();


	//FORCEINLINE UComboActionData* GetComboActionData() const { return ComboActionData; }
protected:

	UFUNCTION()
	virtual void OnOutOfHealth();

protected:
	

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UComboActionData> ComboActionData;*/

	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> DeadMontage;

};
