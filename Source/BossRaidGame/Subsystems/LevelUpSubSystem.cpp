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

	// .ini 에서 설정한 경로의 애셋들을 비동기 로드합니다.
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
	FLevelStatRow* StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*RowName, TEXT("ShowLevelUpChoices"));
	if (!StatData) return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	const UPlayerCharacterAttributeSet* MyAttributeSet = ASC->GetSet<UPlayerCharacterAttributeSet>();
	if (!MyAttributeSet) return;

	TArray<FStatChoiceInfo> AvailableChoices;

	if (StatData->AttackPower > 0)
	{
		FStatChoiceInfo Choice(FText::FromString(TEXT("AttackPower Up")), StatData->AttackPower, UPlayerCharacterAttributeSet::GetAttackPowerAttribute());
		AvailableChoices.Add(Choice);
	}
	if (StatData->AttackSpeed > 0)
	{
		FStatChoiceInfo Choice(FText::FromString(TEXT("AttackSpeed Up")), StatData->AttackSpeed, UPlayerCharacterAttributeSet::GetAttackSpeedAttribute());
		AvailableChoices.Add(Choice);
	}
	if (StatData->SkillPower > 0)
	{
		FStatChoiceInfo Choice(FText::FromString(TEXT("SkillPower Up")), StatData->SkillPower, UPlayerCharacterAttributeSet::GetSkillPowerAttribute());
		AvailableChoices.Add(Choice);
	}
	if (StatData->SkillCooldownRate > 0) // 여기서 비교 연산자를 > 0 으로 통일했습니다. (기존 >= 0)
	{
		FStatChoiceInfo Choice(FText::FromString(TEXT("Skill Cool down")), StatData->SkillCooldownRate, UPlayerCharacterAttributeSet::GetSkillCooldownRateAttribute());
		AvailableChoices.Add(Choice);
	}
	if (StatData->MoveSpeed > 0)
	{
		FStatChoiceInfo Choice(FText::FromString(TEXT("MoveSpeed Up")), StatData->MoveSpeed, UPlayerCharacterAttributeSet::GetMoveSpeedAttribute());
		AvailableChoices.Add(Choice);
	}

	Algo::RandomShuffle(AvailableChoices);

	TArray<FStatChoiceInfo> FinalChoices;
	for (int i = 0; i < FMath::Min(3, AvailableChoices.Num()); ++i)
	{
		FinalChoices.Add(AvailableChoices[i]);
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
<<<<<<< Updated upstream
=======
			
			CurrentStatChoiceWidget->AddToViewport();
			//GamePausedSetting(true);
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;
>>>>>>> Stashed changes

			CurrentStatChoiceWidget->AddToViewport();
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
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