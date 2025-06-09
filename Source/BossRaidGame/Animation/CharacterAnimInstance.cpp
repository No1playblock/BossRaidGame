// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Math/Vector.h"
UCharacterAnimInstance::UCharacterAnimInstance()
{
	//Character = Cast<ACharacter>(GetOwningActor());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	FVector ForwardVector = GetOwningActor()->GetActorForwardVector();
	FVector Velocity = GetOwningActor()->GetVelocity().GetSafeNormal();

	DotNum = FVector::DotProduct(ForwardVector, Velocity);
	CrossNum = FVector::CrossProduct(ForwardVector, Velocity).Z;

    if (DotNum > 0.7f)  // 전방 이동
    {
        CharacterDirection = EDirectionState::Forward;
    }
    else if (DotNum < -0.7f)     // 후방 이동
    {
        CharacterDirection = EDirectionState::BackWard;
    }
    else
    {
        // 좌/우 판별은 Cross.Z 로
        if (CrossNum > 0)    // 오른쪽
        {
            CharacterDirection = EDirectionState::Right;
        }

        else if (CrossNum < 0) // 왼쪽
        {
            CharacterDirection = EDirectionState::Left;
        }
        
    }

}
