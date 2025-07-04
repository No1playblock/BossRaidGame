#include "LevelUpSubsystem.h"
#include "Character/GASCharacterPlayer.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "GameData/LevelStatData.h"
#include "UI/LevelUpScreenWidget.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Algo/RandomShuffle.h"

void ULevelUpSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Warning, TEXT("LevelUpSubsystem: Initializing LevelUpSubsystem."));
	// .ini 에서 설정한 경로의 애셋들을 비동기 로드합니다.
	FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
	if (!LevelStatDataTablePath.IsNull())
	{
		Streamable.RequestAsyncLoad(LevelStatDataTablePath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnDataTableLoaded));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LevelUpSubsystem: LevelStatDataTablePath is not valid."));
	}
	if (!StatChoiceWidgetClassPath.IsNull())
	{
		Streamable.RequestAsyncLoad(StatChoiceWidgetClassPath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnWidgetClassLoaded));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LevelUpSubsystem: StatChoiceWidgetClassPath is not valid."));
	}
}

void ULevelUpSubsystem::OnDataTableLoaded()
{
	LevelStatDataTable = LevelStatDataTablePath.Get();
	if(!LevelStatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelUpSubsystem: Failed to load LevelStatDataTable."));
		return;
	}
}

void ULevelUpSubsystem::OnWidgetClassLoaded()
{
	StatChoiceWidgetClass = StatChoiceWidgetClassPath.Get();
	if(!StatChoiceWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelUpSubsystem: Failed to load StatChoiceWidgetClass."));
		return;
	}
}

void ULevelUpSubsystem::ShowLevelUpChoices(AGASCharacterPlayer* PlayerCharacter, int32 NewLevel)
{
	if (!LevelStatDataTable || !StatChoiceWidgetClass || !PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelUpSubsystem: Required data or widget class is not valid."));
		return;
	}

	TargetPlayer = PlayerCharacter;

	const FString RowName = FString::FromInt(NewLevel);

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	const UPlayerCharacterAttributeSet* MyAttributeSet = ASC->GetSet<UPlayerCharacterAttributeSet>();
	if (!MyAttributeSet) return;

	TArray<EStatChoiceType> StatPool;
	StatPool.Add(EStatChoiceType::AttackPower);
	StatPool.Add(EStatChoiceType::AttackSpeed);
	StatPool.Add(EStatChoiceType::SkillPower);
	StatPool.Add(EStatChoiceType::SkillCooldownRate);
	StatPool.Add(EStatChoiceType::MoveSpeed);

	Algo::RandomShuffle(StatPool);

	TArray<FStatChoiceInfo> FinalChoices;
	int32 ChoiceCount = FMath::Min(3, StatPool.Num());
	for (int i = 0; i < ChoiceCount; ++i)
	{
		EStatChoiceType SelectedStatType = StatPool[i];
		int32 CurrentLevel = 0;
		int32 NextLevel = 0;
		FLevelStatRow* StatData = nullptr;
		FText LevelText;

		// switch 문을 사용해 선택된 enum에 따라 각기 다른 로직을 처리합니다.
		switch (SelectedStatType)						//나중에 좀 더 확장성 있게 짤것. (스탯이 추가될때마다 코드를 추가해야됨)
		{
		case EStatChoiceType::AttackPower:
			CurrentLevel = MyAttributeSet->GetAttackPowerLevel();
			NextLevel = CurrentLevel + 1;
			StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*FString::FromInt(NextLevel), "");
			if (StatData && StatData->AttackPower > 0)
			{
				LevelText = FText::FromString(FString::Printf(TEXT("Lv.%d -> Lv.%d"), CurrentLevel, NextLevel));
				FinalChoices.Add(FStatChoiceInfo(FText::FromString(TEXT("Attack Power")), LevelText, StatData->AttackPower,
					UPlayerCharacterAttributeSet::GetAttackPowerAttribute(), UPlayerCharacterAttributeSet::GetAttackPowerLevelAttribute()));
			}
			break;

		case EStatChoiceType::AttackSpeed:
			CurrentLevel = MyAttributeSet->GetAttackSpeedLevel();
			NextLevel = CurrentLevel + 1;
			StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*FString::FromInt(NextLevel), "");
			if (StatData && StatData->AttackSpeed > 0)
			{
				LevelText = FText::FromString(FString::Printf(TEXT("Lv.%d -> Lv.%d"), CurrentLevel, NextLevel));
				FinalChoices.Add(FStatChoiceInfo(FText::FromString(TEXT("Attack Speed")), LevelText, StatData->AttackSpeed,
					UPlayerCharacterAttributeSet::GetAttackSpeedAttribute(), UPlayerCharacterAttributeSet::GetAttackSpeedLevelAttribute()));
			}
			break;

		case EStatChoiceType::SkillPower:
			CurrentLevel = MyAttributeSet->GetSkillPowerLevel();
			NextLevel = CurrentLevel + 1;
			StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*FString::FromInt(NextLevel), "");
			if (StatData && StatData->SkillPower > 0)
			{
				LevelText = FText::FromString(FString::Printf(TEXT("Lv.%d -> Lv.%d"), CurrentLevel, NextLevel));
				FinalChoices.Add(FStatChoiceInfo(FText::FromString(TEXT("Skill Power")), LevelText, StatData->SkillPower,
					UPlayerCharacterAttributeSet::GetSkillPowerAttribute(), UPlayerCharacterAttributeSet::GetSkillPowerLevelAttribute()));
			}
			break;

		case EStatChoiceType::SkillCooldownRate:
			CurrentLevel = MyAttributeSet->GetSkillCooldownRateLevel();
			NextLevel = CurrentLevel + 1;
			StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*FString::FromInt(NextLevel), "");
			if (StatData && StatData->SkillCooldownRate > 0)
			{
				LevelText = FText::FromString(FString::Printf(TEXT("Lv.%d -> Lv.%d"), CurrentLevel, NextLevel));
				FinalChoices.Add(FStatChoiceInfo(FText::FromString(TEXT("CoolTime Down")), LevelText, StatData->SkillCooldownRate,
					UPlayerCharacterAttributeSet::GetSkillCooldownRateAttribute(), UPlayerCharacterAttributeSet::GetSkillCooldownRateLevelAttribute()));
			}
			break;

		case EStatChoiceType::MoveSpeed:
			CurrentLevel = MyAttributeSet->GetMoveSpeedLevel();
			NextLevel = CurrentLevel + 1;
			StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*FString::FromInt(NextLevel), "");
			if (StatData && StatData->MoveSpeed > 0)
			{
				LevelText = FText::FromString(FString::Printf(TEXT("Lv.%d -> Lv.%d"), CurrentLevel, NextLevel));
				FinalChoices.Add(FStatChoiceInfo(FText::FromString(TEXT("MoveSpeed")), LevelText, StatData->MoveSpeed,
					UPlayerCharacterAttributeSet::GetMoveSpeedAttribute(), UPlayerCharacterAttributeSet::GetMoveSpeedLevelAttribute()));
			}
			break;

		default:
			break;
		}


	}

	APlayerController* PC = PlayerCharacter->GetController<APlayerController>();
	if (PC && FinalChoices.Num() > 0)
	{
		if (CurrentStatChoiceWidget)
		{
			CurrentStatChoiceWidget->RemoveFromParent();
			CurrentStatChoiceWidget = nullptr;
		}

		CurrentStatChoiceWidget = CreateWidget(PC, StatChoiceWidgetClass);
		if (CurrentStatChoiceWidget)
		{
			if (ULevelUpScreenWidget* ChoiceScreen = Cast<ULevelUpScreenWidget>(CurrentStatChoiceWidget))
			{
				ChoiceScreen->InitializeChoices(FinalChoices, PlayerCharacter);
			}

			CurrentStatChoiceWidget->AddToViewport();
			//GamePausedSetting(true);
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;

		}
	}
}

void ULevelUpSubsystem::ApplyStatChoice(const FStatChoiceInfo& ChoiceInfo)
{
	if (!TargetPlayer.IsValid()) return;

	UAbilitySystemComponent* ASC = TargetPlayer->GetAbilitySystemComponent();
	if (ASC)
	{
		UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("StatChoiceEffect")));
		Effect->DurationPolicy = EGameplayEffectDurationType::Instant;

		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = ChoiceInfo.TargetAttribute;
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ChoiceInfo.ModifierValue);
		Effect->Modifiers.Add(ModifierInfo);

		if (ChoiceInfo.TargetLevelAttribute.IsValid())
		{
			FGameplayModifierInfo LevelModifier;
			LevelModifier.Attribute = ChoiceInfo.TargetLevelAttribute;
			LevelModifier.ModifierOp = EGameplayModOp::Additive;
			LevelModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(1.0f); // 레벨은 1씩 증가
			Effect->Modifiers.Add(LevelModifier);
		}


		ASC->ApplyGameplayEffectToSelf(Effect, 1.0f, ASC->MakeEffectContext());
	}

	if (CurrentStatChoiceWidget)
	{
		CurrentStatChoiceWidget->RemoveFromParent();
		CurrentStatChoiceWidget = nullptr;
	}

	APlayerController* PC = TargetPlayer->GetController<APlayerController>();
	if (PC)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}