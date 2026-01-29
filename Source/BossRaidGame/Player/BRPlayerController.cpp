// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Blueprint/UserWidget.h" 
#include "UI/SkillTreeWidget.h"      
#include "UI/PlayerHUDWidget.h"	
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Character/GASCharacterPlayer.h"
#include "UI/JoyStickWidget.h"
#include "HAL/IConsoleManager.h"
#include "UI/InventoryMainWidget.h"
#include "Components/InventoryComponent.h"
#include "Components/QuickSlotComponent.h"
#include "Game/BRGameModeBase.h"
#include "UI/GameOverWidget.h"
ABRPlayerController::ABRPlayerController()
{
	MoveFingerIndex = -1;
	LookFingerIndex = -1;

}

void ABRPlayerController::ShowGameOverUI()
{
	if (!GameOverWidgetInstance)
	{
		// 클래스가 설정되어 있다면
		if (!GameOverWidgetClass.IsNull())
		{
			// 동기 로딩
			UClass* LoadedClass = GameOverWidgetClass.LoadSynchronous();
			if (LoadedClass)
			{
				GameOverWidgetInstance = CreateWidget<UUserWidget>(this, LoadedClass);
			}
		}
	}

	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->AddToViewport(10);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void ABRPlayerController::HideGameOverUI()
{
	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr; 
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ABRPlayerController::RequestRestartGame()
{
	if (ABRGameModeBase* GM = Cast<ABRGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->RestartCurrentLevel();
	}
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}
void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	APawn* MyPawn = GetPawn();
 
	UQuickSlotComponent* MyQuickSlotComponent = MyPawn->FindComponentByClass<UQuickSlotComponent>();
	if(MyQuickSlotComponent)
	{
		QuickSlotComponent = MyQuickSlotComponent;
	}


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
		// 모바일
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
		// PC 입력
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

	if (bIsLooking) // 모바일 시점 변환
	{
		const FVector2D CurrentTouchLocation = Value.Get<FVector2D>();
		FVector2D LookDelta = CurrentTouchLocation - LastLookLocation;
		LastLookLocation = CurrentTouchLocation;

		// 캐릭터가 아닌 컨트롤러의 회전을 직접 제어
		AddYawInput(LookDelta.X * LookSensitivity);
		AddPitchInput(LookDelta.Y * LookSensitivity * -1.0f);
	}
	else // PC일 때
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
		EnhancedInput->BindAction(IA_Inventory, ETriggerEvent::Started, this, &ABRPlayerController::ToggleInventoryUI);
		//EnhancedInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &ABRPlayerController::Attack);

		
		
#if PLATFORM_ANDROID || PLATFORM_IOS
		//Touch는 EnhancedInputComponent를 사용하지 않고 별도 바인딩
		if (InputComponent)
		{
			InputComponent->BindTouch(IE_Pressed, this, &ABRPlayerController::OnTouchBegan);
			InputComponent->BindTouch(IE_Released, this, &ABRPlayerController::OnTouchEnded);
			InputComponent->BindTouch(IE_Repeat, this, &ABRPlayerController::OnTouchMoved); // Moved는 IE_Repeat에 해당
		}
#else
		if (IA_QuickSlot_1) EnhancedInput->BindAction(IA_QuickSlot_1, ETriggerEvent::Started, this, &ABRPlayerController::OnQuickSlot1);
		if (IA_QuickSlot_2) EnhancedInput->BindAction(IA_QuickSlot_2, ETriggerEvent::Started, this, &ABRPlayerController::OnQuickSlot2);
		if (IA_QuickSlot_3) EnhancedInput->BindAction(IA_QuickSlot_3, ETriggerEvent::Started, this, &ABRPlayerController::OnQuickSlot3);
		if (IA_QuickSlot_4) EnhancedInput->BindAction(IA_QuickSlot_4, ETriggerEvent::Started, this, &ABRPlayerController::OnQuickSlot4);
#endif
	}
}

void ABRPlayerController::OnQuickSlot1()
{
	if (QuickSlotComponent) QuickSlotComponent->ActivateSlot(0); // 0번 인덱스
}

void ABRPlayerController::OnQuickSlot2()
{
	if (QuickSlotComponent) QuickSlotComponent->ActivateSlot(1);
}

void ABRPlayerController::OnQuickSlot3()
{
	if (QuickSlotComponent) QuickSlotComponent->ActivateSlot(2);
}

void ABRPlayerController::OnQuickSlot4()
{
	if (QuickSlotComponent) QuickSlotComponent->ActivateSlot(3);
}

void ABRPlayerController::OnTouchBegan(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// 뷰포트 크기 획득
	FVector2D ViewportSize;
	if (GetWorld() && GetWorld()->GetGameViewport())
	{
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	}

	const FVector2D TouchLocation(Location.X, Location.Y);
	if (TouchLocation.X < ViewportSize.X / 2.0f)
	{
		// 왼쪽 화면 터치: 이동용 핑거로 지정
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
		// 오른쪽 화면 터치: 시점 조작용 핑거로 지정
		if (LookFingerIndex == -1)
		{
			LookFingerIndex = FingerIndex;
			LastLookLocation = TouchLocation;
		}
	}
}

// 터치 이동 시 호출될 함수
void ABRPlayerController::OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	const FVector2D TouchLocation(Location.X, Location.Y);

	if (FingerIndex == MoveFingerIndex)
	{
		// 이동용 핑거일 경우 조이스틱 업데이트
		if (MobileHUDInstance)
		{
			MobileHUDInstance->HandleTouchMove(TouchLocation);
		}
	}
	else if (FingerIndex == LookFingerIndex)
	{
		// 시점 조작용 핑거일 경우 시점 회전
		const FVector2D LookDelta = TouchLocation - LastLookLocation;
		LastLookLocation = TouchLocation;

		AddYawInput(LookDelta.X * LookSensitivity);
		AddPitchInput(LookDelta.Y * LookSensitivity);
	}
}

// 터치 종료 시 호출될 함수
void ABRPlayerController::OnTouchEnded(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (FingerIndex == MoveFingerIndex)
	{
		// 이동용 핑거가 떼어졌다면 초기화 및 조이스틱 숨김
		MoveFingerIndex = -1;
		if (MobileHUDInstance)
		{
			MobileHUDInstance->HandleTouchRelease();
		}
	}
	else if (FingerIndex == LookFingerIndex)
	{
		// 시점 조작용 핑거가 떼어졌다면 초기화
		LookFingerIndex = -1;
	}
}
void ABRPlayerController::ToggleSkillTreeUI()
{

	if (!SkillTreeWidgetInstance)
	{
		//클래스 정보가 유효한지 확인
		if (!SkillTreeWidgetClass.IsNull())
		{
			UClass* LoadedClass = SkillTreeWidgetClass.LoadSynchronous();

			if (LoadedClass)
			{
				SkillTreeWidgetInstance = CreateWidget<USkillTreeWidget>(this, LoadedClass);
			}
		}
	}

	if (SkillTreeWidgetInstance)
	{
		const bool bVisible = SkillTreeWidgetInstance->IsInViewport();
		if (!bVisible)
		{
		
			SkillTreeWidgetInstance->AddToViewport();
			SetShowMouseCursor(true);						//여기서 CallStack

			// 탭키만 UI 입력에 포함
			SetInputMode(FInputModeUIOnly());
			SkillTreeWidgetInstance->SetKeyboardFocus();

		}
	}
}
void ABRPlayerController::ToggleInventoryUI()
{
	//없으면 Create
	if (!InventoryWidgetInstance)
	{
		//클래스 정보가 유효한지 확인
		if (!InventoryWidgetClass.IsNull())
		{

			UClass* LoadedClass = InventoryWidgetClass.LoadSynchronous();

			if (LoadedClass)
			{
				InventoryWidgetInstance = CreateWidget<UInventoryMainWidget>(this, LoadedClass);
			}
		}
	}

	//있으면 여닫
	if (InventoryWidgetInstance)
	{
		if (InventoryWidgetInstance->IsInViewport())	//닫기
		{
			InventoryWidgetInstance->RemoveFromParent();

			SetShowMouseCursor(false);
			SetInputMode(FInputModeGameOnly());
		}
		else			//열기
		{
			InventoryWidgetInstance->AddToViewport();

			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				UInventoryComponent* InvComp = ControlledPawn->FindComponentByClass<UInventoryComponent>();
				if (InvComp)
				{
					// 위젯에 컴포넌트 넘겨주고 초기화
					InventoryWidgetInstance->InitInventory(InvComp);
				}
			}

			// 마우스 보이고 UI 입력 가능하게 설정
			SetShowMouseCursor(true);

			// 드래그 앤 드롭을 하려면 GameAndUI가 좋음
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}