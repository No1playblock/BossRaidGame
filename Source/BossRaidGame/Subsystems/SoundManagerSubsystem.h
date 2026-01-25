// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API USoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayBGM(USoundBase* NewBGM, float FadeTime = 1.0f);

    // BGM 정지
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopBGM(float FadeTime = 1.0f);

protected:
    // 실제로 소리를 재생해줄 컴포넌트
    UPROPERTY()
    TObjectPtr<class UAudioComponent> BGMComponent;

    // 현재 재생 중인 음악 기억
    UPROPERTY()
    TObjectPtr<USoundBase> CurrentBGM;
};
