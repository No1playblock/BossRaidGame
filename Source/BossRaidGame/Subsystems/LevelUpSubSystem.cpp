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

	// .ini ���� ������ ����� �ּµ��� �񵿱� �ε��մϴ�.
	FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();
	if (LevelStatDataTablePath.IsValid())
	{
		Streamable.RequestAsyncLoad(LevelStatDataTablePath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnDataTableLoaded));
	}
	if (StatChoiceWidgetClassPath.IsValid())
	{
		Streamable.RequestAsyncLoad(StatChoiceWidgetClassPath.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnWidgetClassLoaded));
	}
}

void ULevelUpSubsystem::OnDataTableLoaded()
{
	LevelStatDataTable = LevelStatDataTablePath.Get();
}

void ULevelUpSubsystem::OnWidgetClassLoaded()
{
	StatChoiceWidgetClass = StatChoiceWidgetClassPath.Get();
}

void ULevelUpSubsystem::ShowLevelUpChoices(AGASCharacterPlayer* PlayerCharacter, int32 NewLevel)
{
	if (!LevelStatDataTable || !StatChoiceWidgetClass || !PlayerCharacter)
	{
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

		// switch ���� ����� ���õ� enum�� ���� ���� �ٸ� ������ ó���մϴ�.
		switch (SelectedStatType)						//���߿� �� �� Ȯ�强 �ְ� ©��. (������ �߰��ɶ����� �ڵ带 �߰��ؾߵ�)
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