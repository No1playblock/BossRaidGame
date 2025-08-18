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
    
    const FRotator CurrentRotation = PawnOwner->GetActorRotation();
    CurrentYaw = CurrentRotation.Yaw;

    IsRotating = !FMath::IsNearlyEqual(CurrentYaw, PreviousYaw);
    
    // 다음 프레임에서의 비교를 위해 현재 Yaw 값을 PreviousYaw에 저장합니다.
    PreviousYaw = CurrentYaw;
}
