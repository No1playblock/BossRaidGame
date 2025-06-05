// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Attribute/CharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

ANonPlayerGASCharacter::ANonPlayerGASCharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ANonPlayerGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ANonPlayerGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
}
void ANonPlayerGASCharacter::OnOutOfHealth()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
	
	SetActorEnableCollision(false);

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), 5.0f, false);

}