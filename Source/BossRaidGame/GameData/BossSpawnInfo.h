#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BossSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct FBossSpawnInfo : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 스폰될 보스 캐릭터 블루프린트
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ACharacter> BossClass;

    // 보스 등장 시 사용할 이펙트 (VFX)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UParticleSystem* SpawnVFX;

    // 보스 등장 시 사용할 사운드 (SFX)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USoundBase* SpawnSFX;
};