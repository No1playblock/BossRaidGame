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
#include "GameData/StatChoiceData.h"

void ULevelUpSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("LevelUpSubsystem::Initialize() Call"));
	UE_LOG(LogTemp, Warning, TEXT("LevelStatDataTablePath: %s"), *LevelStatDataTablePath.ToString());
	UE_LOG(LogTemp, Warning, TEXT("StatChoiceWidgetClassPath: %s"), *StatChoiceWidgetClassPath.ToString());


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
	UE_LOG(LogTemp, Warning, TEXT("ULevelUpSubsystemInitialize"));
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
	
	if (!LevelStatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelStatDataTable"));
		
	}
	if (!StatChoiceWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("StatChoiceWidgetClass"));
		
	}

	if (!LevelStatDataTable || !StatChoiceWidgetClass || !PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("NULLNULL"));
		UE_LOG(LogTemp, Warning, TEXT("Assets not loaded yet. Requesting async load and deferring ShowLevelUpChoices."));

		// 람다 함수에서 사용하기 위해 파라미터를 복사해둡니다.
		TWeakObjectPtr<AGASCharacterPlayer> PlayerCharacterPtr = PlayerCharacter;

		FStreamableManager& Streamable = UAssetManager::Get().GetStreamableManager();

		// 로드 완료 시 실행될 델리게이트를 생성합니다.
		FStreamableDelegate DelegateToExecute;
		DelegateToExecute.BindLambda([this, PlayerCharacterPtr, NewLevel]()
			{
				// 로드가 완료되었으니, ShowLevelUpChoices 함수를 다시 호출합니다.
				if (PlayerCharacterPtr.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("Async load complete. Retrying ShowLevelUpChoices."));
					ShowLevelUpChoices(PlayerCharacterPtr.Get(), NewLevel);
				}
			});

		// 두 애셋의 경로를 리스트에 담습니다.
		TArray<FSoftObjectPath> AssetsToLoad;
		AssetsToLoad.Add(LevelStatDataTablePath.ToSoftObjectPath());
		AssetsToLoad.Add(StatChoiceWidgetClassPath.ToSoftObjectPath());

		// 애셋 리스트를 비동기 로드하고, 모든 로딩이 끝나면 위의 델리게이트를 실행하도록 요청합니다.
		Streamable.RequestAsyncLoad(AssetsToLoad, DelegateToExecute);
		return;
	}


	TargetPlayer = PlayerCharacter;

	const FString RowName = FString::FromInt(NewLevel);
	/*FLevelStatRow* StatData = LevelStatDataTable->FindRow<FLevelStatRow>(*RowName, TEXT("ShowLevelUpChoices"));
	if (!StatData) return;
	UE_LOG(LogTemp, Warning, TEXT("StatData"));*/

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;
	UE_LOG(LogTemp, Warning, TEXT("ASC"));

	const UPlayerCharacterAttributeSet* MyAttributeSet = ASC->GetSet<UPlayerCharacterAttributeSet>();
	if (!MyAttributeSet) return;
	UE_LOG(LogTemp, Warning, TEXT("MyAttributeSet"));

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
		switch (SelectedStatType)
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
			GamePausedSetting(true);
			/*UGameplayStatics::SetGamePaused(GetWorld(), true);
			PC->SetInputMode(FInputModeUIOnly());
			PC->bShowMouseCursor = true;*/

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

		UGameplayEffect* LevelValueEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("StatLevelEffect")));
		LevelValueEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		FGameplayModifierInfo LevelModifier;
		LevelModifier.Attribute = ChoiceInfo.TargetLevelAttribute; // FStatChoiceInfo에 추가했던 레벨 어트리뷰트 사용
		LevelModifier.ModifierOp = EGameplayModOp::Additive;
		LevelModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(1.0f); // 레벨은 1씩 증가
		LevelValueEffect->Modifiers.Add(LevelModifier);

		ASC->ApplyGameplayEffectToSelf(LevelValueEffect, 1.0f, ASC->MakeEffectContext());

	}
	APlayerController* PC = TargetPlayer->GetController<APlayerController>();
	if (PC)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}


	if (CurrentStatChoiceWidget)
	{
		CurrentStatChoiceWidget->RemoveFromParent();
		CurrentStatChoiceWidget = nullptr;
	}

	
}
void ULevelUpSubsystem::GamePausedSetting(bool bValue)
{
	FTimerHandle PauseTimerHandle; 

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, bValue]() 
		{
			// 이 안의 코드가 0.1초 뒤에 실행됩니다.
			if (!TargetPlayer.IsValid()) return;

			APlayerController* PC = TargetPlayer->GetController<APlayerController>();
			if (PC)
			{
				UGameplayStatics::SetGamePaused(PC->GetWorld(), bValue);
				PC->bShowMouseCursor = bValue;
				PC->SetInputMode(FInputModeUIOnly());
			}
		});

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UWorld* World = LocalPlayer->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				PauseTimerHandle,
				TimerDelegate,  // FTimerDelegate 객체 전달
				0.7f,           // 0.1초 딜레이
				false           // 반복 안 함
			);
		}
	}
}
