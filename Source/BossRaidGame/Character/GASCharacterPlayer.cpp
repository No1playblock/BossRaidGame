// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GASCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/BRPlayerState.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkillTreeComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/QuickSlotComponent.h"
#include "Tag/BRGameplayTag.h"
#include "EnhancedInputSubsystems.h"
#include "Game/BRGameModeBase.h"
AGASCharacterPlayer::AGASCharacterPlayer()
{
	ASC = nullptr;
	AttributeSet = CreateDefaultSubobject<UPlayerCharacterAttributeSet>(TEXT("AttributeSet"));
	SkillTreeComponent = CreateDefaultSubobject<USkillTreeComponent>(TEXT("SkillTreeComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	QuickSlotComponent = CreateDefaultSubobject<UQuickSlotComponent>(TEXT("QuickSlotComponent"));
}

UAbilitySystemComponent* AGASCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ABRPlayerState* BRPS = GetPlayerState<ABRPlayerState>();
	if (BRPS)
	{

		ASC = BRPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(BRPS, this);

		const UPlayerCharacterAttributeSet* RealAttributeSet = ASC->GetSet<UPlayerCharacterAttributeSet>();
		if (RealAttributeSet)
		{
			AttributeSet = const_cast<UPlayerCharacterAttributeSet*>(RealAttributeSet);

			AttributeSet->OnOutOfHealth.AddUObject(this, &AGASCharacterPlayer::OnOutOfHealth);

		}

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
		//PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
		//PlayerController->ConsoleCommand(TEXT("stat unit"));
		//PlayerController->ConsoleCommand(TEXT("stat fps"));
			
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
		AttributeSet->SetMoveSpeed(700.0f);
	}
	
	
}

void AGASCharacterPlayer::SetInteractableActor(AActor* Interactable)
{
	CurrentInteractableActor = Interactable;
}

void AGASCharacterPlayer::ClearInteractableActor(AActor* Interactable)
{
	if (CurrentInteractableActor == Interactable)
	{
		CurrentInteractableActor = nullptr;
	}
}

void AGASCharacterPlayer::ActivateAbilityByInputID(int32 InputId)
{
	GASInputPressed(InputId);
}

//BossCharacter는 스킬에 따라 데미지가 달라 만들어진 메소드
float AGASCharacterPlayer::GetDamageByAttackTag(const FGameplayTag& AttackTag) const
{
	if (AttributeSet)
	{
		// 플레이어는 항상 자신의 AttackPower를 데미지로 사용
		return AttributeSet->GetAttackPower();
	}
	return 0.0f;
}

void AGASCharacterPlayer::OnOutOfHealth()
{
	Super::OnOutOfHealth();
	if (ASC)
	{
		ASC->CancelAllAbilities();
		ASC->AddLooseGameplayTag(BRTAG_CHARACTER_ISDEAD);
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
		}
	}

	if (CameraBoom)		//죽는 연출
	{
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritYaw = false;
		CameraBoom->bInheritRoll = false;
		
		CameraBoom->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
		CameraBoom->TargetArmLength = 800.0f;

		CameraBoom->TargetOffset = FVector::ZeroVector;
		CameraBoom->SocketOffset = FVector::ZeroVector;

		CameraBoom->SetRelativeLocation(FVector(-150.0f, 0.0f, 0.0f));
		if (FollowCamera)
		{
			FollowCamera->SetRelativeLocation(FVector::ZeroVector);

		}

		CameraBoom->bDoCollisionTest = false;

		CameraBoom->bEnableCameraLag = true;
		CameraBoom->bEnableCameraRotationLag = true;

		CameraBoom->CameraLagSpeed = 1.4f;
		CameraBoom->CameraRotationLagSpeed = 1.4f;
	}
	if (ABRGameModeBase* GM = Cast<ABRGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->OnPlayerDied(GetController());
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();

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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGASCharacterPlayer::GASInputReleased, 0);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 1);
		EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 2);
		EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 3);
		EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 4);
		EnhancedInputComponent->BindAction(ShiftSkillAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 5);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AGASCharacterPlayer::GASInputPressed, 6);

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

// UMG의 공격 버튼이 눌리면 이 함수가 호출됨
void AGASCharacterPlayer::RequestPrimaryAttackPressed()
{
	// GAS의 표준 입력 처리 함수를 호출하여 어빌리티 실행
	GASInputPressed(1);
}

// UMG의 공격 버튼에서 손을 떼면 이 함수가 호출됨
void AGASCharacterPlayer::RequestPrimaryAttackReleased()
{
	GASInputReleased(1);
}

void AGASCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC)
	{
		// 등록했던 델리게이트를 핸들을 사용해 안전하게 해제
		ASC->GetGameplayAttributeValueChangeDelegate(UPlayerCharacterAttributeSet::GetMoveSpeedAttribute()).Remove(MoveSpeedChangedDelegateHandle);
	}

	Super::EndPlay(EndPlayReason);
}