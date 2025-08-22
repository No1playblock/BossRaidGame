// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // �÷��̾� ��Ʈ����Ʈ ������ ����
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameData/SkillTreeData.h"
#include "Game/BRGameState.h"

void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());

	if (AbilitySystemComponent.IsValid())
	{
		const UPlayerCharacterAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UPlayerCharacterAttributeSet>();
		if (AttributeSet)
		{
			// ��Ʈ����Ʈ ���� ��������Ʈ ���ε�
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleMaxHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentExpAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleExperienceChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentLevelAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleLevelChanged);
		}

		// ��ٿ� �±� �̺�Ʈ ���ε�
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		//AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		//AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);

		// ���� ��Ƽ���� �ν��Ͻ� ���� �� �ʿ� ����
		if (QSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q")), QSkillMaskImage->GetDynamicMaterial());
		if (ESkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E")), ESkillMaskImage->GetDynamicMaterial());
		//if (ShiftSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift")), ShiftSkillMaskImage->GetDynamicMaterial());
		//if (RSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R")), RSkillMaskImage->GetDynamicMaterial());

		if (ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>())
		{
			GS->OnTimeChanged.AddDynamic(this, &UPlayerHUDWidget::OnTimeChanged);
		}


		// ������ ó�� ������ �� ���� ���� ������ UI�� ��� ������Ʈ
		UpdateAllHUDValues();
	}
}
void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ��ٿ� ���� ��ų���� UI�� �� ������ ������Ʈ
	for (auto const& [Tag, Info] : SkillCooldownInfos)
	{
		if (Info.bIsOnCooldown)
		{
			UpdateCooldownUI(Tag);
		}
	}
}
void UPlayerHUDWidget::UpdateExperienceWidgets()
{
	if (!AbilitySystemComponent.IsValid() || !ExpBar || !LevelText) return;

	const UPlayerCharacterAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UPlayerCharacterAttributeSet>();
	if (!AttributeSet) return;

	const float CurrentExp = AttributeSet->GetCurrentExp();
	UE_LOG(LogTemp, Warning, TEXT("Current Exp: %f"), CurrentExp);
	const int32 CurrentLevel = FMath::FloorToInt(AttributeSet->GetCurrentLevel());

	// ���� �ؽ�Ʈ�� ���� ������ ������Ʈ
	LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevel)));

	//����ġ �ٸ� ������Ʈ.
	// 1������ ��� ���� ����ġ�� 0
	const float ExpForCurrentLevelStart = AttributeSet->GetTotalExpForLevel(CurrentLevel);
	// ���� ������ ���� ����ġ (��: 3������ ������ = 2���� �޼��� �ʿ��� �� ����ġ)
	const float ExpForNextLevelStart = AttributeSet->GetTotalExpForLevel(CurrentLevel+1);

	//1���� 0 2���� 30	

	if (ExpForNextLevelStart > ExpForCurrentLevelStart)
	{
		const float ExpInThisLevel = CurrentExp - ExpForCurrentLevelStart;
		const float ExpNeededForThisLevel = ExpForNextLevelStart - ExpForCurrentLevelStart;

		ExpBar->SetPercent(ExpInThisLevel / ExpNeededForThisLevel);
		//UE_LOG(LogTemp, Warning, TEXT("ExpInThisLevel: %f, ExpNeededForThisLevel: %f"), ExpInThisLevel, ExpNeededForThisLevel);
	}
	else
	{
		// ���� ���� ������ ������ (����) �ٸ� ���� ä��ϴ�.
		ExpBar->SetPercent(1.0f);
	}
}
void UPlayerHUDWidget::SetSkillUI(EAbilityInputID InputID, const FSkillTreeDataRow* Data)
{
	if (!AbilitySystemComponent.IsValid() || Data==nullptr) return;

	//UE_LOG(LogTemp, Warning, TEXT("SetSkillUI called with InputID: %s, SkillIcon: %s"), *UEnum::GetValueAsString(InputID), *Data->SkillIcon.ToString());
	FGameplayTag CooldownTag;
	UImage* TargetSkillIcon = nullptr;
	switch (InputID)
	{
	case EAbilityInputID::Q_Skill:
		TargetSkillIcon = QSkillIcon;
		CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q"));

		break;
	case EAbilityInputID::E_Skill:
		TargetSkillIcon = ESkillIcon;
		CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E"));

		break;
	case EAbilityInputID::R_Skill:
		TargetSkillIcon = RSkillIcon;
		CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R"));

		break;
	case EAbilityInputID::Shift_Skill:
		TargetSkillIcon = ShiftSkillIcon;
		CooldownTag = FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift"));

		break;
	default:
		return; // �������� �ʴ� InputID�� ���
	}
	if (TargetSkillIcon)
	{
		TargetSkillIcon->SetBrushFromSoftTexture(Data->SkillIcon);
	}
	if (CooldownTag.IsValid())
	{
		SkillCooldownDurations.Add(CooldownTag, Data->SkillCoolTime);
	}
}
void UPlayerHUDWidget::UpdateAllHUDValues()
{
	if (AbilitySystemComponent.IsValid())
	{
		const UPlayerCharacterAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UPlayerCharacterAttributeSet>();
		if (AttributeSet)
		{
			// ü�� ������ ���� ������ ���� �� ��������Ʈ ȣ��
			FOnAttributeChangeData HealthData;
			HealthData.NewValue = AttributeSet->GetHealth();
			HandleHealthChanged(HealthData);

			// ����ġ ������ ���� ������ ���� �� ��������Ʈ ȣ��
			FOnAttributeChangeData ExpData;
			ExpData.NewValue = AttributeSet->GetCurrentExp();
			HandleExperienceChanged(ExpData);

			// ���� ������ ���� ������ ���� �� ��������Ʈ ȣ��
			FOnAttributeChangeData LevelData;
			LevelData.NewValue = AttributeSet->GetCurrentLevel();
			HandleLevelChanged(LevelData);
		}
	}
}


void UPlayerHUDWidget::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (AbilitySystemComponent.IsValid() && HpBar && CurrentHealthText && MaxHealthText)
	{
		const float CurrentHealth = Data.NewValue;
		const float MaxHealth = AbilitySystemComponent->GetSet<UPlayerCharacterAttributeSet>()->GetMaxHealth();
		const float HealthPercent = MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;

		HpBar->SetPercent(HealthPercent);
		CurrentHealthText->SetText(FText::AsNumber(FMath::CeilToInt(CurrentHealth)));
		MaxHealthText->SetText(FText::AsNumber(FMath::CeilToInt(MaxHealth)));
	}
}

void UPlayerHUDWidget::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if (AbilitySystemComponent.IsValid() && HpBar && CurrentHealthText && MaxHealthText)
	{
		const float CurrentHealth = AbilitySystemComponent->GetSet<UPlayerCharacterAttributeSet>()->GetHealth();
		const float MaxHealth = Data.NewValue;
		const float HealthPercent = MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;

		HpBar->SetPercent(HealthPercent);
		CurrentHealthText->SetText(FText::AsNumber(FMath::CeilToInt(CurrentHealth)));
		MaxHealthText->SetText(FText::AsNumber(FMath::CeilToInt(MaxHealth)));
	}
}

void UPlayerHUDWidget::HandleExperienceChanged(const FOnAttributeChangeData& Data)
{
	UpdateExperienceWidgets();
	//UE_LOG(LogTemp, Warning, TEXT("Experience changed: New Value = %.2f"), Data.NewValue);
}

void UPlayerHUDWidget::HandleLevelChanged(const FOnAttributeChangeData& Data)
{
	UpdateExperienceWidgets();
}
void UPlayerHUDWidget::OnCooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (!AbilitySystemComponent.IsValid()) return;

	// �ش� �±��� ��ٿ� ������ �������ų� ���� �߰�
	FSkillCooldownInfo& CooldownInfo = SkillCooldownInfos.FindOrAdd(CooldownTag);

	if (NewCount > 0) // ��ٿ� ����
	{
		CooldownInfo.bIsOnCooldown = true;
		CooldownInfo.CooldownStartTime = GetWorld()->GetTimeSeconds();

		if (SkillCooldownDurations.Contains(CooldownTag))
		{
			CooldownInfo.CooldownDuration = SkillCooldownDurations[CooldownTag];
		}
		else
		{
			CooldownInfo.CooldownDuration = 0.f; // Ȥ�� �� ��츦 ����� �⺻��
		}
	}
	else // ��ٿ� ����
	{
		CooldownInfo.bIsOnCooldown = false;
	}

	// ���°� ����� ��� UI ������Ʈ
	UpdateCooldownUI(CooldownTag);
}

void UPlayerHUDWidget::UpdateCooldownUI(const FGameplayTag& CooldownTag)
{
	if (!AbilitySystemComponent.IsValid()) return;

	const FSkillCooldownInfo& CooldownInfo = SkillCooldownInfos.FindOrAdd(CooldownTag);

	UImage* TargetMaskImage = nullptr;
	UTextBlock* TargetTextBlock = nullptr;
	//UE_LOG(LogTemp, Warning, TEXT("UpdateCooldownUI called for CooldownTag: %s"), *CooldownTag.ToString());
	if (CooldownTag == FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q")))
	{
		TargetMaskImage = QSkillMaskImage;
		TargetTextBlock = QSkillCooldownText;
	}
	else if (CooldownTag == FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E")))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UpdateCooldownUI called for E Skill with CooldownTag: %s"), *CooldownTag.ToString());
		TargetMaskImage = ESkillMaskImage;
		TargetTextBlock = ESkillCooldownText;
	}
	else if (CooldownTag == FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R")))
	{
		TargetMaskImage = RSkillMaskImage;
		TargetTextBlock = RSkillCooldownText;
	}
	else if (CooldownTag == FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift")))
	{
		TargetMaskImage = ShiftSkillMaskImage;
		TargetTextBlock = ShiftSkillCooldownText;
	}


	if (TargetTextBlock && TargetMaskImage && CooldownMaterials.Contains(CooldownTag))
	{
		UMaterialInstanceDynamic* MID = CooldownMaterials[CooldownTag];
		if (!MID) return;

		if (CooldownInfo.bIsOnCooldown)
		{
			const float TimeElapsed = GetWorld()->GetTimeSeconds() - CooldownInfo.CooldownStartTime;
			const float TimeRemaining = CooldownInfo.CooldownDuration - TimeElapsed;
			
			if (TimeRemaining > 0.f)
			{
				TargetMaskImage->SetVisibility(ESlateVisibility::Visible);
				TargetTextBlock->SetVisibility(ESlateVisibility::Visible);
				const float Progress = FMath::Clamp(TimeElapsed / CooldownInfo.CooldownDuration, 0.0f, 50.0f);
				MID->SetScalarParameterValue(FName("Progress"), Progress);
				TargetTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), TimeRemaining)));
			}
			else
			{
				// Ȥ�� �� ������ ���� Tick������ ���� ó��
				TargetMaskImage->SetVisibility(ESlateVisibility::Hidden);
				TargetTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			TargetMaskImage->SetVisibility(ESlateVisibility::Hidden);
			TargetTextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void UPlayerHUDWidget::OnTimeChanged(int32 NewTime)
{
	if (TimerText)
	{
		// ��:�� �������� ��ȯ�Ͽ� �ؽ�Ʈ ������Ʈ
		const int32 Minutes = NewTime / 60;
		const int32 Seconds = NewTime % 60;
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		TimerText->SetText(FText::FromString(TimeString));
	}
}