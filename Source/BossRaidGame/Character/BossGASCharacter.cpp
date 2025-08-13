// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BossGASCharacter.h"
#include "Attribute/MobCharacterAttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameData/BossSpawnInfo.h"
TArray<FGameplayTag> ABossGASCharacter::GetMeleeAttackTags() const
{
	TArray<FGameplayTag> MeleeTags;
	for (const FAttackData& Attack : MeleeAttacks)
	{
		MeleeTags.Add(Attack.AttackTag);
	}
	return MeleeTags;
}
TArray<FGameplayTag> ABossGASCharacter::GetRangedAttackTags() const
{
	TArray<FGameplayTag> RangedTags;
	for (const FAttackData& Attack : RangedAttacks)
	{
		RangedTags.Add(Attack.AttackTag);
	}
	return RangedTags;
}
void ABossGASCharacter::InitializeFromData(const FBossSpawnInfo* BossData)
{
	if (!BossData || !AttributeSet || !GetCharacterMovement()) return;

	// 체력 설정
	// AttributeSet의 부모인 UCharacterAttributeSet에 Health, MaxHealth가 있다고 가정
	ASC->SetNumericAttributeBase(AttributeSet->GetMaxHealthAttribute(), BossData->Health);
	ASC->SetNumericAttributeBase(AttributeSet->GetHealthAttribute(), BossData->Health);

	// 경험치 보상 설정
	ASC->SetNumericAttributeBase(AttributeSet->GetExpRewardAttribute(), BossData->Experience);
	
	MeleeAttacks = BossData->MeleeAttacks;
	RangedAttacks = BossData->RangedAttacks;

}

float ABossGASCharacter::GetDamageByAttackTag(const FGameplayTag& AttackTag) const
{
	for (const FAttackData& Attack : MeleeAttacks)
	{
		if (Attack.AttackTag == AttackTag)
		{
			return Attack.Damage;
		}
	}

	for (const FAttackData& Attack : RangedAttacks)
	{
		if (Attack.AttackTag == AttackTag)
		{
			return Attack.Damage;
		}
	}
	return 0.f;
}
