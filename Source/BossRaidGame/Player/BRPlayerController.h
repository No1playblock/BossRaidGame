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
	void ToggleSkillTreeUI(); // UI�� ���ݴ� �Լ�

	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void Jump();
	void StopJumping();

protected:

	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Desktop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Mobile;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ToggleSkillTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;

	
	UPROPERTY()
	class UJoyStickWidget* MobileHUDInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UJoyStickWidget> MobileHUDWidgetClass;


	UPROPERTY()
	class UPlayerHUDWidget* PlayerHUDWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	USkillTreeWidget* SkillTreeWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<USkillTreeWidget> SkillTreeWidgetClass;

private:

	void OnTouchBegan(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchEnded(const ETouchIndex::Type FingerIndex, const FVector Location);


	// -- ����� ���̽�ƽ ���� ���� --
	int32 MoveFingerIndex;
	int32 LookFingerIndex;

	bool bIsLooking = false;
	FVector2D LastLookLocation;

	//���� ����
	float LookSensitivity = 0.1f;
};
