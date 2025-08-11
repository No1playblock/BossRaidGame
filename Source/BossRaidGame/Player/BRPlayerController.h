// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BRPlayerController.generated.h"

/**
 * 
 */
class USkillTreeWidget;

UCLASS()
class BOSSRAIDGAME_API ABRPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABRPlayerController();

	FORCEINLINE class UPlayerHUDWidget* GetPlayerHUDWidgetInstance() const { return PlayerHUDWidgetInstance; }
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void ToggleSkillTreeUI(); // UI를 여닫는 함수

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ToggleSkillTree;

	
	UPROPERTY()
	class UPlayerHUDWidget* PlayerHUDWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	USkillTreeWidget* SkillTreeWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<USkillTreeWidget> SkillTreeWidgetClass;

};
