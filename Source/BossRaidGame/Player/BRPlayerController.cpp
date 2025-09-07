// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Blueprint/UserWidget.h" 
#include "UI/SkillTreeWidget.h"      
#include "UI/PlayerHUDWidget.h"	
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Character/GASCharacterPlayer.h"
#include "UI/JoyStickWidget.h"
ABRPlayerController::ABRPlayerController()
{
	MoveFingerIndex = -1;
	LookFingerIndex = -1;

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputAction/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Object)
	{
		IA_Jump = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputAction/IA_Move.IA_Move'"));
	if (MoveActionRef.Object)
	{
		IA_Move = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputAction/IA_Look.IA_Look'"));
	if (LookActionRef.Object)
	{
		IA_Look = LookActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputAction/IA_Attack.IA_Attack'"));
	if (AttackActionRef.Object)
	{
		IA_Attack = AttackActionRef.Object;
	}

	static ConstructorHelpers::FClassFinder<USkillTreeWidget> SkillTreeWidgetBPClass(TEXT("/Game/UI/WBP_SkillTree.WBP_SkillTree_C"));
	if (SkillTreeWidgetBPClass.Succeeded())
	{
		SkillTreeWidgetClass = SkillTreeWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UPlayerHUDWidget> PlayerHUDWidgetBPClass(TEXT("/Game/UI/WBP_PlayerHUDWidget.WBP_PlayerHUDWidget_C"));
	if (PlayerHUDWidgetBPClass.Succeeded())
	{
		PlayerHUDWidgetClass = PlayerHUDWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UJoyStickWidget> PlayerMobileHUDWidgetBPClass(TEXT("/Game/UI/WBP_VirtualJoystick.WBP_VirtualJoystick_C"));
	if (PlayerMobileHUDWidgetBPClass.Succeeded())
	{
		MobileHUDWidgetClass = PlayerMobileHUDWidgetBPClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleSkillTreeAction(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/InputAction/IA_ToggleSkillUI.IA_ToggleSkillUI'"));
	if (ToggleSkillTreeAction.Succeeded())
	{
		IA_ToggleSkillTree = ToggleSkillTreeAction.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DesktopMappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (DesktopMappingContext.Succeeded())
	{
		IMC_Desktop = DesktopMappingContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MobileMappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Mobile.IMC_Mobile'"));
	if (MobileMappingContext.Succeeded())
	{
		IMC_Mobile = MobileMappingContext.Object;
	}
}

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("PlayerController BeginPlay"));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();

#if PLATFORM_ANDROID || PLATFORM_IOS
		if (IMC_Mobile) Subsystem->AddMappingContext(IMC_Mobile, 0);
		if (MobileHUDWidgetClass)
		{
			MobileHUDInstance = CreateWidget<UJoyStickWidget>(this, MobileHUDWidgetClass);
			if (MobileHUDInstance)
			{
				MobileHUDInstance->AddToViewport();
			}
		}
#else
		if (IMC_Desktop) Subsystem->AddMappingContext(IMC_Desktop, 0);

#endif
	}

	if (!PlayerHUDWidgetInstance && PlayerHUDWidgetClass)
	{
		PlayerHUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDWidgetClass);
	}
	if (PlayerHUDWidgetInstance)
	{
		PlayerHUDWidgetInstance->AddToViewport();
	}

}
void ABRPlayerController::Move(const FInputActionValue& Value)
{
	if (MobileHUDInstance && MobileHUDInstance->IsVisible())
	{
		// �����
		const FVector2D MoveVec = MobileHUDInstance->GetMoveVector();
		if (!MoveVec.IsNearlyZero())
		{
			APawn* MyPawn = GetPawn();
			if (MyPawn)
			{
				const FRotator ControlRot = GetControlRotation();
				const FVector ForwardDir = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::X);
				const FVector RightDir = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

				MyPawn->AddMovementInput(ForwardDir, MoveVec.Y);
				MyPawn->AddMovementInput(RightDir, MoveVec.X);
			}
		}
	}
	else
	{
		// PC �Է�
		const FVector2D MoveVec = Value.Get<FVector2D>();
		APawn* MyPawn = GetPawn();
		if (MyPawn)
		{
			const FRotator ControlRot = GetControlRotation();
			const FVector ForwardDir = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::X);
			const FVector RightDir = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

			MyPawn->AddMovementInput(ForwardDir, MoveVec.X);
			MyPawn->AddMovementInput(RightDir, MoveVec.Y);
		}
	}
}
void ABRPlayerController::Look(const FInputActionValue& Value)
{
	ACharacterPlayer* MyPawn = GetPawn<ACharacterPlayer>();
	if (!MyPawn) return;

	if (bIsLooking) // ����� ���� ��ȯ
	{
		const FVector2D CurrentTouchLocation = Value.Get<FVector2D>();
		FVector2D LookDelta = CurrentTouchLocation - LastLookLocation;
		LastLookLocation = CurrentTouchLocation;

		// ĳ���Ͱ� �ƴ� ��Ʈ�ѷ��� ȸ���� ���� ����
		AddYawInput(LookDelta.X * LookSensitivity);
		AddPitchInput(LookDelta.Y * LookSensitivity * -1.0f);
	}
	else // PC�� ��
	{
		const FVector2D LookInput = Value.Get<FVector2D>();
		AddYawInput(LookInput.X);
		AddPitchInput(LookInput.Y);
	}
}
void ABRPlayerController::Jump()
{
	if (ACharacterPlayer* MyPawn = GetPawn<ACharacterPlayer>())
	{
		MyPawn->Jump();
	}
}

void ABRPlayerController::StopJumping()
{
	if (ACharacterPlayer* MyPawn = GetPawn<ACharacterPlayer>())
	{
		MyPawn->StopJumping();
	}
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{

		EnhancedInput->BindAction(IA_ToggleSkillTree, ETriggerEvent::Started, this, &ABRPlayerController::ToggleSkillTreeUI);
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ABRPlayerController::Move);
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ABRPlayerController::Look);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &ABRPlayerController::Jump);
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ABRPlayerController::StopJumping);
		//EnhancedInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &ABRPlayerController::Attack);

		//Touch�� EnhancedInputComponent�� ������� �ʰ� ���� ���ε�
		if (InputComponent)
		{
			InputComponent->BindTouch(IE_Pressed, this, &ABRPlayerController::OnTouchBegan);
			InputComponent->BindTouch(IE_Released, this, &ABRPlayerController::OnTouchEnded);
			InputComponent->BindTouch(IE_Repeat, this, &ABRPlayerController::OnTouchMoved); // Moved�� IE_Repeat�� �ش�
		}

	}
}
void ABRPlayerController::OnTouchBegan(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// ����Ʈ ũ�� ȹ��
	FVector2D ViewportSize;
	if (GetWorld() && GetWorld()->GetGameViewport())
	{
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	}

	const FVector2D TouchLocation(Location.X, Location.Y);
	if (TouchLocation.X < ViewportSize.X / 2.0f)
	{
		// ���� ȭ�� ��ġ: �̵��� �ΰŷ� ����
		if (MoveFingerIndex == -1)
		{
			MoveFingerIndex = FingerIndex;
			if (MobileHUDInstance)
			{
				MobileHUDInstance->HandleTouchPress(TouchLocation);
			}
		}
	}
	else
	{
		// ������ ȭ�� ��ġ: ���� ���ۿ� �ΰŷ� ����
		if (LookFingerIndex == -1)
		{
			LookFingerIndex = FingerIndex;
			LastLookLocation = TouchLocation;
		}
	}
}

// ��ġ �̵� �� ȣ��� �Լ�
void ABRPlayerController::OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	const FVector2D TouchLocation(Location.X, Location.Y);

	if (FingerIndex == MoveFingerIndex)
	{
		// �̵��� �ΰ��� ��� ���̽�ƽ ������Ʈ
		if (MobileHUDInstance)
		{
			MobileHUDInstance->HandleTouchMove(TouchLocation);
		}
	}
	else if (FingerIndex == LookFingerIndex)
	{
		// ���� ���ۿ� �ΰ��� ��� ���� ȸ��
		const FVector2D LookDelta = TouchLocation - LastLookLocation;
		LastLookLocation = TouchLocation;

		AddYawInput(LookDelta.X * LookSensitivity);
		AddPitchInput(LookDelta.Y * LookSensitivity);
	}
}

// ��ġ ���� �� ȣ��� �Լ�
void ABRPlayerController::OnTouchEnded(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex == MoveFingerIndex)
	{
		// �̵��� �ΰŰ� �������ٸ� �ʱ�ȭ �� ���̽�ƽ ����
		MoveFingerIndex = -1;
		if (MobileHUDInstance)
		{
			MobileHUDInstance->HandleTouchRelease();
		}
	}
	else if (FingerIndex == LookFingerIndex)
	{
		// ���� ���ۿ� �ΰŰ� �������ٸ� �ʱ�ȭ
		LookFingerIndex = -1;
	}
}
void ABRPlayerController::ToggleSkillTreeUI()
{
	if (!SkillTreeWidgetInstance && SkillTreeWidgetClass)
	{
		SkillTreeWidgetInstance = CreateWidget<USkillTreeWidget>(this, SkillTreeWidgetClass);
	}
	if (SkillTreeWidgetInstance)
	{
		const bool bVisible = SkillTreeWidgetInstance->IsInViewport();
		if (!bVisible)
		{
			SkillTreeWidgetInstance->AddToViewport();
			SetShowMouseCursor(true);						//���⼭ CallStack
			// ��Ű�� UI �Է¿� ����
			SetInputMode(FInputModeUIOnly());
			SkillTreeWidgetInstance->SetKeyboardFocus();
			//UGameplayStatics::SetGamePaused(GetWorld(), true);

		}
	}
}