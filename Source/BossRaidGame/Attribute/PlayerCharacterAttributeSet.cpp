// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PlayerCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/GASCharacterPlayer.h"
#include "Engine/CurveTable.h"
#include "SubSystems/LevelUpSubSystem.h"

UPlayerCharacterAttributeSet::UPlayerCharacterAttributeSet() :
	CurrentExp(0.0f),
	CurrentLevel(0.0f),
	SkillPower(10.0f),
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

			if (NewLevel > GetCurrentLevel())		//레벨업 된 상황
			{
				SetCurrentLevel(NewLevel);
				SetSkillPoint(GetSkillPoint() + 1);
				if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
				{
					// AvatarActor를 우리의 플레이어 캐릭터 클래스로 캐스팅
					AGASCharacterPlayer* PlayerCharacter = Cast<AGASCharacterPlayer>(Data.Target.AbilityActorInfo->AvatarActor.Get());
					APlayerController* PC = PlayerCharacter ? PlayerCharacter->GetController<APlayerController>() : nullptr;
					ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;

					if (LocalPlayer && PlayerCharacter)
					{
						// 로컬 플레이어로부터 서브시스템을 가져옴
						if (ULevelUpSubsystem* LevelUpSubsystem = LocalPlayer->GetSubsystem<ULevelUpSubsystem>())
						{
							// 서브시스템의 함수를 호출
							LevelUpSubsystem->ShowLevelUpChoices(PlayerCharacter, NewLevel);
						}
					}
				}
			}
		}
		
	}
}
