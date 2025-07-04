// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Blueprint/UserWidget.h" // CreateWidget 사용
#include "UI/SkillTreeWidget.h"      // SkillTreeWidget 참조 (수정한 부분입니다)
#include "Kismet/GameplayStatics.h" 
ABRPlayerController::ABRPlayerController()
{
	static ConstructorHelpers::FClassFinder<USkillTreeWidget> SkillTreeWidgetBPClass(TEXT("/Game/Blueprints/UI/WBP_SkillTree.WBP_SkillTree_C"));
	if (SkillTreeWidgetBPClass.Succeeded())
	{
		SkillTreeWidgetClass = SkillTreeWidgetBPClass.Class; // 수정한 부분입니다.
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleSkillTreeAction(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/InputAction/IA_ToggleSkillUI.IA_ToggleSkillUI'"));
	if (ToggleSkillTreeAction.Succeeded())
	{
		IA_ToggleSkillTree = ToggleSkillTreeAction.Object; // 수정한 부분입니다.
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Input/IMC_Default.IMC_Default'"));
	if (MappingContext.Succeeded())
	{
		DefaultMappingContext = MappingContext.Object; // 수정한 부분입니다.
	}
}

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 수정한 부분입니다. EnhancedInputSubsystem에 Mapping 추가
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(IA_ToggleSkillTree, ETriggerEvent::Triggered, this, &ABRPlayerController::ToggleSkillTreeUI);
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
			SetShowMouseCursor(true);
			// 탭키만 UI 입력에 포함
			SetInputMode(FInputModeUIOnly());
			SkillTreeWidgetInstance->SetKeyboardFocus();
			//UGameplayStatics::SetGamePaused(GetWorld(), true);

		}
	}
}