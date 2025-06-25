// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkillTreeComponent.h"
#include "GameData/SkillTreeData.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FSkillTreeDataRow* USkillTreeComponent::GetSkillData(const FName& SkillID) const
{
	if (!SkillDataTable) return nullptr;

	return SkillDataTable->FindRow<FSkillTreeDataRow>(SkillID, TEXT(""), true);
}

const FSkillTreeDataRow* USkillTreeComponent::FindSkillDataByGrantedAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!SkillDataTable || !AbilityClass) return nullptr;

	for (const FName& SkillID : AcquiredSkills)
	{
		const FSkillTreeDataRow* Row = SkillDataTable->FindRow<FSkillTreeDataRow>(SkillID, TEXT(""));
		if (Row && Row->GrantedAbility == AbilityClass)
		{
			return Row;
		}
	}
	return nullptr;
}