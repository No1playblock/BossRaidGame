// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealingPool.generated.h"


class UBoxComponent;
class UParticleSystemComponent;
class UGameplayEffect;

UCLASS()
class BOSSRAIDGAME_API AHealingPool : public AActor
{
	GENERATED_BODY()
	
public:
	AHealingPool();

protected:
	virtual void BeginPlay() override;

	// 주기적으로 힐링을 적용하기 위해 호출될 함수입니다.
	UFUNCTION()
	void ApplyHealing();

	// 충돌 및 범위 감지를 위한 스피어 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	// 시각 효과를 위한 파티클 시스템 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	// 블루프린트에서 설정할, 적용할 힐링 게임플레이 이펙트 클래스입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> HealingEffectClass;

	// 힐링이 적용되는 간격 (초) 입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float HealingInterval = 1.0f;

	// 힐링 장판의 총 수명 (초) 입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float PoolLifetime = 10.0f;

private:
	// 힐링 타이머를 위한 핸들입니다.
	FTimerHandle HealingTimerHandle;

};
