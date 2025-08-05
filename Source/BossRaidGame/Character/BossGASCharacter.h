// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/NonPlayerGASCharacter.h"
#include "BossGASCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API ABossGASCharacter : public ANonPlayerGASCharacter
{
	GENERATED_BODY()

public:
	FORCEINLINE const TArray<FGameplayTag>& GetMeleeAttackTags() const { return MeleeAttackTags; }
	FORCEINLINE const TArray<FGameplayTag>& GetRangedAttackTags() const { return RangedAttackTags; }
	
protected:
    // 이 보스가 가진 근접 공격 어빌리티 태그 목록
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FGameplayTag> MeleeAttackTags;

    // 이 보스가 가진 원거리 공격 어빌리티 태그 목록
    UPROPERTY(EditDefaultsOnly, Category = "AI | Attacks")
    TArray<FGameplayTag> RangedAttackTags;
};
