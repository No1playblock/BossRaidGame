// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */

UCLASS()
class BOSSRAIDGAME_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCharacterAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	class ACharacter* Character;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsRotating;

private:

	float CurrentYaw;

	float PreviousYaw;

	
};
