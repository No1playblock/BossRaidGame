// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Blueprint/UserWidget.h" // CreateWidget ���
#include "UI/SkillTreeWidget.h"      // SkillTreeWidget ���� (������ �κ��Դϴ�)
#include "Kismet/GameplayStatics.h" 
ABRPlayerController::ABRPlayerController()
{
	static ConstructorHelpers::FClassFinder<USkillTreeWidget> SkillTreeWidgetBPClass(TEXT("/Game/Blueprints/UI/WBP_SkillTree.WBP_SkillTree_C"));
	if (SkillTreeWidgetBPClass.Succeeded())
	{
		SkillTreeWidgetClass = SkillTreeWidgetBPClass.Class; // ������ �κ��Դϴ�.
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleSkillTreeAction(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/InputAction/IA_ToggleSkillUI.IA_ToggleSkillUI'"));
	if (ToggleSkillTreeAction.Succeeded())
	{
		IA_ToggleSkillTree = ToggleSkillTreeAction.Object; // ������ �κ��Դϴ�.
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Input/IMC_Default.IMC_Default'"));
	if (MappingContext.Succeeded())
	{
		DefaultMappingContext = MappingContext.Object; // ������ �κ��Դϴ�.
	}
}

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ������ �κ��Դϴ�. EnhancedInputSubsystem�� Mapping �߰�
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
			// ��Ű�� UI �Է¿� ����
			SetInputMode(FInputModeUIOnly());
			SkillTreeWidgetInstance->SetKeyboardFocus();
			//UGameplayStatics::SetGamePaused(GetWorld(), true);

		}
	}
}