// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaserOrb.generated.h"

UCLASS()
class BOSSRAIDGAME_API ARaserOrb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaserOrb();

	UFUNCTION(BlueprintCallable)
	void FireLaser();

	void Initialize(AActor* InInstigator, float InDamage, TSubclassOf<class UGameplayEffect> NewDamageEffectClass, TEnumAsByte<ECollisionChannel> NewTargetChannel);

protected:
	virtual void BeginPlay() override;

private:
	FHitResult LineTrace() const;

	void SelfDestroy();

	UPROPERTY()
	AActor* InstigatorActor;

	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	float DamageAmount = 0.0f;

	TEnumAsByte<ECollisionChannel> TargetChannel;
};
