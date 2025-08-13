// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/NonPlayerGASCharacter.h"
#include "BossGASCharacter.generated.h"

/**
 * 
 */
struct FBossSpawnInfo; 
struct FAttackData;
UCLASS()
class BOSSRAIDGAME_API ABossGASCharacter : public ANonPlayerGASCharacter
{
	GENERATED_BODY()

public:
    TArray<FGameplayTag> GetMeleeAttackTags() const;
    TArray<FGameplayTag> GetRangedAttackTags() const;
	
    void InitializeFromData(const FBossSpawnInfo* BossData);

    //ANonPlayerGASCharacter���� ���
    virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const override;

protected:
    // �� ������ ���� ���� ���� �����Ƽ �±� ���
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FAttackData> MeleeAttacks;

    // �� ������ ���� ���Ÿ� ���� �����Ƽ �±� ���
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FAttackData> RangedAttacks;
};
