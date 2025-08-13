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

    //ANonPlayerGASCharacter에서 상속
    virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const override;

protected:
    // 이 보스가 가진 근접 공격 어빌리티 태그 목록
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FAttackData> MeleeAttacks;

    // 이 보스가 가진 원거리 공격 어빌리티 태그 목록
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FAttackData> RangedAttacks;
};
