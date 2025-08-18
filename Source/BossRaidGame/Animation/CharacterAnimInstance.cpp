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

    // 초기 Yaw 값을 설정하여 첫 프레임부터 올바르게 작동하도록 함
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
    const FRotator CurrentRotation = PawnOwner->GetActorRotation();
    CurrentYaw = CurrentRotation.Yaw;

    IsRotating = !FMath::IsNearlyEqual(CurrentYaw, PreviousYaw);
    
    // 다음 프레임에서의 비교를 위해 현재 Yaw 값을 PreviousYaw에 저장합니다.
    PreviousYaw = CurrentYaw;
}
