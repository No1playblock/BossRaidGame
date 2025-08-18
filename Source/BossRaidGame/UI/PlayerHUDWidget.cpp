// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PlayerCharacterAttributeSet.h" // 플레이어 어트리뷰트 셋으로 변경
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
			// 어트리뷰트 변경 델리게이트 바인딩
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleMaxHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentExpAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleExperienceChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentLevelAttribute()).AddUObject(this, &UPlayerHUDWidget::HandleLevelChanged);
		}

		// 쿨다운 태그 이벤트 바인딩
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		//AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);
		//AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerHUDWidget::OnCooldownTagChanged);

		// 동적 머티리얼 인스턴스 생성 및 맵에 저장
		if (QSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Q")), QSkillMaskImage->GetDynamicMaterial());
		if (ESkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.E")), ESkillMaskImage->GetDynamicMaterial());
		//if (ShiftSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.Shift")), ShiftSkillMaskImage->GetDynamicMaterial());
		//if (RSkillMaskImage) CooldownMaterials.Add(FGameplayTag::RequestGameplayTag(FName("Data.Cooldown.Skill.R")), RSkillMaskImage->GetDynamicMaterial());

		if (ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>())
		{
			GS->OnTimeChanged.AddDynamic(this, &UPlayerHUDWidget::OnTimeChanged);
		}


		// 위젯이 처음 생성될 때 현재 스탯 값으로 UI를 즉시 업데이트
		UpdateAllHUDValues();
	}
}
void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 쿨다운 중인 스킬들의 UI를 매 프레임 업데이트
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

	// 레벨 텍스트를 현재 레벨로 업데이트
	LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevel)));

	//경험치 바를 업데이트.
	// 1레벨일 경우 시작 경험치는 0
	const float ExpForCurrentLevelStart = AttributeSet->GetTotalExpForLevel(CurrentLevel);
	// 다음 레벨의 시작 경험치 (예: 3레벨의 시작점 = 2레벨 달성에 필요한 총 경험치)
	const float ExpForNextLevelStart = AttributeSet->GetTotalExpForLevel(CurrentLevel+1);

	//1레벨 0 2레벨 30	

	if (ExpForNextLevelStart > ExpForCurrentLevelStart)
	{
		const float ExpInThisLevel = CurrentExp - ExpForCurrentLevelStart;
		const float ExpNeededForThisLevel = ExpForNextLevelStart - ExpForCurrentLevelStart;

		ExpBar->SetPercent(ExpInThisLevel / ExpNeededForThisLevel);
		UE_LOG(LogTemp, Warning, TEXT("ExpInThisLevel: %f, ExpNeededForThisLevel: %f"), ExpInThisLevel, ExpNeededForThisLevel);
	}
	else
	{
		// 다음 레벨 정보가 없으면 (만렙) 바를 가득 채웁니다.
		ExpBar->SetPercent(1.0f);
	}
}
void UPlayerHUDWidget::SetSkillUI(EAbilityInputID InputID, const FSkillTreeDataRow* Data)
{
	if (!AbilitySystemComponent.IsValid() || Data==nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("SetSkillUI called with InputID: %s, SkillIcon: %s"), *UEnum::GetValueAsString(InputID), *Data->SkillIcon.ToString());
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
		return; // 지원하지 않는 InputID인 경우
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
			// 체력 정보로 가상 데이터 생성 후 델리게이트 호출
			FOnAttributeChangeData HealthData;
			HealthData.NewValue = AttributeSet->GetHealth();
			HandleHealthChanged(HealthData);

			// 경험치 정보로 가상 데이터 생성 후 델리게이트 호출
			FOnAttributeChangeData ExpData;
			ExpData.NewValue = AttributeSet->GetCurrentExp();
			HandleExperienceChanged(ExpData);

			// 레벨 정보로 가상 데이터 생성 후 델리게이트 호출
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
	UE_LOG(LogTemp, Warning, TEXT("Experience changed: New Value = %.2f"), Data.NewValue);
}

void UPlayerHUDWidget::HandleLevelChanged(const FOnAttributeChangeData& Data)
{
	UpdateExperienceWidgets();
}
void UPlayerHUDWidget::OnCooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (!AbilitySystemComponent.IsValid()) return;

	// 해당 태그의 쿨다운 정보를 가져오거나 새로 추가
	FSkillCooldownInfo& CooldownInfo = SkillCooldownInfos.FindOrAdd(CooldownTag);

	if (NewCount > 0) // 쿨다운 시작
	{
		CooldownInfo.bIsOnCooldown = true;
		CooldownInfo.CooldownStartTime = GetWorld()->GetTimeSeconds();

		if (SkillCooldownDurations.Contains(CooldownTag))
		{
			CooldownInfo.CooldownDuration = SkillCooldownDurations[CooldownTag];
		}
		else
		{
			CooldownInfo.CooldownDuration = 0.f; // 혹시 모를 경우를 대비한 기본값
		}
	}
	else // 쿨다운 종료
	{
		CooldownInfo.bIsOnCooldown = false;
	}

	// 상태가 변경된 즉시 UI 업데이트
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
				// 혹시 모를 오차를 위해 Tick에서도 숨김 처리
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
		// 분:초 형식으로 변환하여 텍스트 업데이트
		const int32 Minutes = NewTime / 60;
		const int32 Seconds = NewTime % 60;
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		TimerText->SetText(FText::FromString(TimeString));
	}
}