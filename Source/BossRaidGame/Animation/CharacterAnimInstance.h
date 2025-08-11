// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDirectionState : uint8
{
	Forward        UMETA(DisplayName = "Forward"),
	BackWard     UMETA(DisplayName = "BackWard"),
	Left     UMETA(DisplayName = "Left"),
	Right   UMETA(DisplayName = "Right")
};

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
	float DotNum;
	float CrossNum;

	class ACharacter* Character;

	UPROPERTY(BlueprintReadWrite)
	EDirectionState CharacterDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool IsRotating;
private:

	float CurrentYaw;

	float PreviousYaw;
};
