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
	SkillPower(2.0f),
	SkillCooldownRate(1.0f)
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
void UPlayerCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UPlayerCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentExpAttribute())
	{
		if (GetCurrentExp() != 0)
		{
			const FRealCurve* Curve = LevelTable->FindCurve(FName("CriterionLevelValue"), FString());
			if (!Curve) return;

			float EvalLevel = Curve->Eval(GetCurrentExp());
			int32 NewLevel = FMath::FloorToInt(EvalLevel);

			if (NewLevel > GetCurrentLevel())		//������ �� ��Ȳ
			{
				SetCurrentLevel(NewLevel);
				SetSkillPoint(GetSkillPoint() + 1);
				SetMaxHealth(GetMaxHealth() + 100.f); // ������ �� ü�� ����
				SetHealth(GetMaxHealth()); // ü�� ȸ��
				if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
				{
					AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(Data.Target.AbilityActorInfo->AvatarActor.Get());
					APlayerController* PC = PlayerCharacter ? PlayerCharacter->GetController<APlayerController>() : nullptr;
					ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;

					if (LocalPlayer && PlayerCharacter)
					{
						// ���� �÷��̾�κ��� ����ý����� ������
						if (ULevelUpSubsystem* LevelUpSubsystem = LocalPlayer->GetSubsystem<ULevelUpSubsystem>())
						{
							// ����ý����� �Լ��� ȣ��
							LevelUpSubsystem->ShowLevelUpChoices(PlayerCharacter, NewLevel);
						}
					}
				}
			}
		}
		
	}
}
