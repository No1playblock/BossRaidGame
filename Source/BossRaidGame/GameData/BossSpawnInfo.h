#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttackData.h"
#include "BossSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct FBossSpawnInfo : public FTableRowBase
{
    GENERATED_BODY()

public:
    // ������ ���� ĳ���� �������Ʈ
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ACharacter> BossClass;

    // ���� ���� �� ����� ����Ʈ (VFX)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UParticleSystem* SpawnVFX;

    // ���� ���� �� ����� ���� (SFX)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* SpawnSFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float Experience;

    UPROPERTY(EditDefaultsOnly, Category = "Attacks")
    TArray<FAttackData> MeleeAttacks;

    // �� ������ ���� ���Ÿ� ���� �����Ƽ �±� ���
    UPROPERTY(EditDefaultsOnly, Category = "Attacks")
    TArray<FAttackData> RangedAttacks;
};