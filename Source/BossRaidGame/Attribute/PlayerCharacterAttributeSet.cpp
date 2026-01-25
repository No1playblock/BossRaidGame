// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PlayerCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/GASCharacterPlayer.h"
#include "Engine/CurveTable.h"
#include "SubSystems/LevelUpSubSystem.h"
#include "Curves/RichCurve.h"

UPlayerCharacterAttributeSet::UPlayerCharacterAttributeSet() :
	CurrentExp(0.0f),
	CurrentLevel(1.0f),
	SkillPower(1.0f),
	SkillCooldownRate(1.0f),
	SkillPoint(1.0f)
{
	static ConstructorHelpers::FObjectFinder<UCurveTable> LevelTableRef(TEXT("/Script/Engine.CurveTable'/Game/GameData/ExpLevelTable.ExpLevelTable'"));
	if (LevelTableRef.Object)
	{
		LevelTable = LevelTableRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurveTable NULL"));

	}
}
float UPlayerCharacterAttributeSet::GetTotalExpForLevel(int32 Level) const
{
	if (Level <= 0)
	{
		return 0.f;
	}
	if (!LevelTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelTable is not set"));
		return 0.f;
	}
	const FSimpleCurve* SimpleCurve = LevelTable->FindSimpleCurve(
		FName(TEXT("CriterionLevelValue")),
		TEXT("GetTotalExpForLevel")
	);

	if (!SimpleCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Curve row 'CriterionLevelValue' not found in LevelTable"));
		return 0.f;
	}

	for (const FSimpleCurveKey& Key : SimpleCurve->Keys)
	{
		if (FMath::RoundToInt(Key.Value) == Level)
		{
			return Key.Time;
		}
	}

	return 0.f;
}
void UPlayerCharacterAttributeSet::ProcessLevelUp()
{
	if (GetCurrentExp() == 0 || !LevelTable) return;

	AGASCharacterPlayer* Player = nullptr;
	ULevelUpSubsystem* LevelSubsystem = nullptr;

	if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		Player = Cast<AGASCharacterPlayer>(ASC->GetAvatarActor());
		if (Player)
		{
			if (APlayerController* PC = Player->GetController<APlayerController>())
			{
				if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
				{
					LevelSubsystem = LocalPlayer->GetSubsystem<ULevelUpSubsystem>();
				}
			}
		}
	}

	// UI가 이미 떠있으면 리턴 (위에서 구한 Subsystem 재사용)
	if (LevelSubsystem && LevelSubsystem->IsLevelUpWidgetShown())
	{
		return;
	}

	const FRealCurve* Curve = LevelTable->FindCurve(FName("CriterionLevelValue"), TEXT("ProcessLevelUp"));
	if (!Curve) return;

	float EvalLevel = Curve->Eval(GetCurrentExp());
	int32 TargetLevel = FMath::FloorToInt(EvalLevel);
	int32 MyCurrentLevel = GetCurrentLevel();

	// 레벨업 조건 충족 시
	if (TargetLevel > MyCurrentLevel)
	{
		int32 NextLevel = MyCurrentLevel + 1;
		SetCurrentLevel(NextLevel);

		SetSkillPoint(GetSkillPoint() + 1);
		SetMaxHealth(GetMaxHealth() + 100.f);
		SetHealth(GetMaxHealth());

		// 미리 구해둔 변수로 즉시 호출
		if (LevelSubsystem && Player)
		{
			LevelSubsystem->ShowLevelUpChoices(Player, NextLevel);
		}
	}
}
void UPlayerCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UPlayerCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentExpAttribute())
	{
		ProcessLevelUp();
	}
	
}
