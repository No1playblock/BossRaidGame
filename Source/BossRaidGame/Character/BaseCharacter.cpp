// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

}

void ABaseCharacter::OnOutOfHealth()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->StopAllMontages(0.0f);
	
	if (DeadMontage)
	{
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
	}
	
	SetActorEnableCollision(false);

}


