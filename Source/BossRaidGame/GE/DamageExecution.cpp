// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/DamageExecution.h"
#include "Attribute/PlayerCharacterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Tag/BRGameplayTag.h"

// 이 실행 계산이 어떤 어트리뷰트를 캡처할지 정의하는 구조체
struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition SkillPowerDef;
	FGameplayEffectAttributeCaptureDefinition DamageDef;

	FDamageStatics()
		// UPlayerCharacterAttributeSet::GetSkillPowerAttribute() 함수를 직접 호출
		// 이 함수는 public static이므로 외부에서 접근 가능
		: SkillPowerDef(UPlayerCharacterAttributeSet::GetSkillPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)

		// UCharacterAttributeSet::GetDamageAttribute() 함수를 직접 호출
		, DamageDef(UCharacterAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	{
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

UDamageExecution::UDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().SkillPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float SourceSkillPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SkillPowerDef, FAggregatorEvaluateParameters(), SourceSkillPower);

	const float BaseDamage = Spec.GetSetByCallerMagnitude(BRTAG_DATA_DAMAGE, false, 0.f);

	const float FinalDamage = BaseDamage * SourceSkillPower;

	//UE_LOG(LogTemp, Warning, TEXT("UDamageExecution::FinalDamage: %f"), FinalDamage);
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
}
