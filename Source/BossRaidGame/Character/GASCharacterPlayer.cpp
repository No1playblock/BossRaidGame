// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/BRPlayerState.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameData/LevelStatData.h"
#include "Blueprint/UserWidget.h"              
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "Kismet/GameplayStatics.h"               // SetGamePaused
#include "GameFramework/PlayerController.h"         // PlayerController 클래스
#include "GameplayEffect.h"                       // UGameplayEffect 클래스
#include "GameplayEffectTypes.h"                  // FGameplayModifierInfo
#include "Algo/RandomShuffle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkillComponent.h"
AGASCharacterPlayer::AGASCharacterPlayer()
{
	ASC = nullptr;
	AttributeSet = CreateDefaultSubobject<UPlayerCharacterAttributeSet>(TEXT("AttributeSet"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

}

UAbilitySystemComponent* AGASCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("PossessdBy"));
	ABRPlayerState* BRPS = GetPlayerState<ABRPlayerState>();
	if (BRPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("BRPS"));


		ASC = BRPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(BRPS, this);

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

		if (ASC)
		{
			
			MoveSpeedChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
				UPlayerCharacterAttributeSet::GetMoveSpeedAttribute()
			).AddLambda([this](const FOnAttributeChangeData& Data)
				{
					if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
					{
						MovementComponent->MaxWalkSpeed = Data.NewValue;
					}
				});

			if (AttributeSet)
			{
				GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMoveSpeed();
			}
		}
		AttributeSet->SetMoveSpeed(400.0f);
	}

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);

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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGASCharacterPlayer::GASInputReleased, 0);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 1);
	}
}

void AGASCharacterPlayer::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
	
}

void AGASCharacterPlayer::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;


		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
	
}
void AGASCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC)
	{
		// 등록했던 델리게이트를 핸들을 사용해 안전하게 해제합니다.
		ASC->GetGameplayAttributeValueChangeDelegate(UPlayerCharacterAttributeSet::GetMoveSpeedAttribute()).Remove(MoveSpeedChangedDelegateHandle);
	}

	Super::EndPlay(EndPlayReason);
}