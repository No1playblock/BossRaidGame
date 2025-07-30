#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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
};