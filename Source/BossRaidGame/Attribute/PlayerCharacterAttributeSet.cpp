// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PlayerCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Engine/CurveTable.h"


UPlayerCharacterAttributeSet::UPlayerCharacterAttributeSet() :
	CurrentExp(0.0f),
	CurrentLevel(0.0f)
{
	static ConstructorHelpers::FObjectFinder<UCurveTable> LevelTableRef(TEXT("/Script/Engine.CurveTable'/Game/Blueprints/GameData/ExpLevelTable.ExpLevelTable'"));
	if (LevelTableRef.Object)
	{
		LevelTable = LevelTableRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurveTable NULL"));

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
		if (GetCurrentExp() != 0)
		{
			UpdateLevelFromExp(GetCurrentExp());
			//SetCurrentLevel((int)(GetCurrentExp() / 10));
			//Data.Target.GetAvatarActor()
		}
		
	}
}
void UPlayerCharacterAttributeSet::UpdateLevelFromExp(float Exp)
{
	/*static const FString CurvePath = TEXT("/Game/Blueprints/GameData/ExpLevelTable.ExpLevelTable");
	static UCurveTable* CurveTable = LoadObject<UCurveTable>(nullptr, *CurvePath);
	if (!CurveTable) return;
	else
		UE_LOG(LogTemp, Warning, TEXT("CurveTable NULL"));*/


	const FRealCurve* Curve = LevelTable->FindCurve(FName("CriterionLevelValue"), FString());
	if (!Curve) return;

	float EvalLevel = Curve->Eval(Exp);
	int32 NewLevel = FMath::FloorToInt(EvalLevel);

	if (NewLevel > GetCurrentLevel())
	{
		SetCurrentLevel(NewLevel);
		//OnLevelUp.Broadcast(NewLevel); // 델리게이트 활용 가능
	}
}