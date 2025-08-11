// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GameData/SkillTreeData.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
struct FOnAttributeChangeData;
struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FSkillCooldownInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bIsOnCooldown = false;

	UPROPERTY(BlueprintReadOnly)
	float CooldownStartTime = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CooldownDuration = 0.f;
};


UCLASS()
class BOSSRAIDGAME_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetSkillUI(EAbilityInputID InputID, const FSkillTreeDataRow* Data);
protected:


	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** HP�� ����Ǿ��� �� �������Ʈ���� ȣ��˴ϴ�. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthChanged(float NewHealth, float MaxHealth);

	/** ����ġ�� ����Ǿ��� �� �������Ʈ���� ȣ��˴ϴ�. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnExperienceChanged(float NewExperience, float MaxExperience);

	/** Ư�� ��ų�� ��ٿ� ���°� ����Ǿ��� �� �������Ʈ���� ȣ��˴ϴ�. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnSkillCooldownChanged(FGameplayTag SkillCooldownTag, float TimeRemaining);


	void UpdateExperienceWidgets();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> HpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ExpBar;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TimerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CurrentHealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MaxHealthText;

	// --- Q ��ų ���� ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> QSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> QSkillMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QSkillCooldownText;

	// --- E ��ų ���� ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ESkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ESkillMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ESkillCooldownText;
	// --- R ��ų ���� ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> RSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> RSkillMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RSkillCooldownText;
	// --- Shift ��ų ���� ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ShiftSkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ShiftSkillMaskImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ShiftSkillCooldownText;
private:

	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);
	void HandleExperienceChanged(const FOnAttributeChangeData& Data);
	void HandleLevelChanged(const FOnAttributeChangeData& Data);
	void OnCooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);
	void UpdateAllHUDValues();
	void UpdateCooldownUI(const FGameplayTag& CooldownTag);

	UFUNCTION()
	void OnTimeChanged(int32 NewTime);

	// ASC�� ���� ���� ������
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// �� ��ų �±׿� �ش��ϴ� ���� ��Ƽ���� �ν��Ͻ��� ������ ��
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UMaterialInstanceDynamic>> CooldownMaterials;

	UPROPERTY()
	TMap<FGameplayTag, FSkillCooldownInfo> SkillCooldownInfos;

	UPROPERTY()
	TMap<FGameplayTag, float> SkillCooldownDurations;
};
