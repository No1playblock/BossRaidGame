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

	UFUNCTION()
	void ApplyHealing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> HealingEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float HealingInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float PoolLifetime = 10.0f;

private:
	FTimerHandle HealingTimerHandle;

};
