// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/BRPlayerState.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"

AGASCharacterPlayer::AGASCharacterPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FollowCamera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* AGASCharacterPlayer::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABRPlayerState* BRPS = GetPlayerState<ABRPlayerState>();
	if (BRPS)
	{
		ASC = BRPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(this, this);

		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}
		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartInputSpec(StartInputAbility.Value);
			StartInputSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartInputSpec);
		}
		SetupGASInputComponent();

		APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}

}

void AGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupGASInputComponent();

}

void AGASCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 0);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGASCharacterPlayer::GASInputReleased, 0);
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 1);
	}
}

void AGASCharacterPlayer::GASInputPressed(int32 InputId)
{
}

void AGASCharacterPlayer::GASInputReleased(int32 InputId)
{
}
