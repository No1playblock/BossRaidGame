// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EffectPoolSubsystem.generated.h"

/**
 *
 */
USTRUCT()
struct FEffectPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UParticleSystemComponent>> Items;
};

UCLASS()
class BOSSRAIDGAME_API UEffectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// UGameplayStatics::SpawnEmitterAtLocation 대신 사용할 함수
	UFUNCTION(BlueprintCallable, Category = "EffectPool")
	UParticleSystemComponent* SpawnEffect(UParticleSystem* EffectTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f));

	UFUNCTION(BlueprintCallable, Category = "EffectPool")
	void WarmUpPool(UParticleSystem* EffectTemplate, int32 Count);
private:
	// 파티클 재생이 끝나면 호출되는 델리게이트
	UFUNCTION()
	void OnParticleSystemFinished(UParticleSystemComponent* PSC);

private:
	// 키: 파티클 에셋, 값: 해당 파티클 컴포넌트들의 배열(스택)
	UPROPERTY()
	TMap<UParticleSystem*, FEffectPoolArray> EffectPool;
};
