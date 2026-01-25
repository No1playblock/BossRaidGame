// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SoundManagerSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void USoundManagerSubsystem::PlayBGM(USoundBase* NewBGM, float FadeTime)
{
    // 이미 같은 음악이 나오고 있으면 무시
    if (BGMComponent && BGMComponent->Sound == NewBGM && BGMComponent->IsPlaying())
    {
        return;
    }

    // 다른 음악이 나오고 있었다면 페이드 아웃 후 정지
    if (BGMComponent)
    {
        BGMComponent->FadeOut(FadeTime, 0.0f);
        BGMComponent = nullptr; // 기존 컴포넌트 참조 해제 (GC가 알아서 처리하거나 풀링)
    }

    // 새 음악 재생
    if (NewBGM)
    {
        // 2D 사운드로 재생
        // SpawnSound2D는 AudioComponent를 반환
        BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), NewBGM);

        if (BGMComponent)
        {
            BGMComponent->bAutoDestroy = false; // BGM은 계속 틀어야 하므로 자동 파괴 방지
            BGMComponent->FadeIn(FadeTime);
            CurrentBGM = NewBGM;
        }
    }
}

void USoundManagerSubsystem::StopBGM(float FadeTime)
{
    if (BGMComponent)
    {
        BGMComponent->FadeOut(FadeTime, 0.0f);
        // FadeOut이 끝나면 자동으로 Stop 되도록 설정
    }
    CurrentBGM = nullptr;
}
