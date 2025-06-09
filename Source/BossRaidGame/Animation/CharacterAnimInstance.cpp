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

    if (DotNum > 0.7f)  // ���� �̵�
    {
        CharacterDirection = EDirectionState::Forward;
    }
    else if (DotNum < -0.7f)     // �Ĺ� �̵�
    {
        CharacterDirection = EDirectionState::BackWard;
    }
    else
    {
        // ��/�� �Ǻ��� Cross.Z ��
        if (CrossNum > 0)    // ������
        {
            CharacterDirection = EDirectionState::Right;
        }

        else if (CrossNum < 0) // ����
        {
            CharacterDirection = EDirectionState::Left;
        }
        
    }

}
