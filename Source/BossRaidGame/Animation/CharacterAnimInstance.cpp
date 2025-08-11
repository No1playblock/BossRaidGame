// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Math/Vector.h"
UCharacterAnimInstance::UCharacterAnimInstance()
{
	//Character = Cast<ACharacter>(GetOwningActor());
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // �ʱ� Yaw ���� �����Ͽ� ù �����Ӻ��� �ùٸ��� �۵��ϵ��� ��
    APawn* PawnOwner = TryGetPawnOwner();
    if (PawnOwner)
    {
        CurrentYaw = PawnOwner->GetActorRotation().Yaw;
        PreviousYaw = CurrentYaw;
    }
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

    APawn* PawnOwner = TryGetPawnOwner();
    if (!PawnOwner)
    {
        return;
    }

	FVector ForwardVector = PawnOwner->GetActorForwardVector();
	FVector Velocity = PawnOwner->GetVelocity().GetSafeNormal();

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
    const FRotator CurrentRotation = PawnOwner->GetActorRotation();
    CurrentYaw = CurrentRotation.Yaw;

    IsRotating = !FMath::IsNearlyEqual(CurrentYaw, PreviousYaw);
    
    // ���� �����ӿ����� �񱳸� ���� ���� Yaw ���� PreviousYaw�� �����մϴ�.
    PreviousYaw = CurrentYaw;
}
