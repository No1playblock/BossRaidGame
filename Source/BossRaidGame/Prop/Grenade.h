// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SkillSpawnedActorInterface.h"
#include "Interface/PoolableInterface.h"
#include "Grenade.generated.h"

UCLASS()
class BOSSRAIDGAME_API AGrenade : public AActor, public ISkillSpawnedActorInterface, public IPoolableInterface
{
	GENERATED_BODY()
	
public:	
	AGrenade();

	virtual void OnPoolSpawned() override;
	virtual void OnPoolDespawned() override;

protected:
	virtual void BeginPlay() override;

	virtual void InitializeInfo_Implementation(float InDamage, TSubclassOf<UGameplayEffect> InDamageEffect) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle ReturnTimerHandle;

	// 타이머가 끝나면 호출될 함수
	void ReturnSelf();


	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> GrenadeMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent> SphereComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<class UParticleSystem> ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	float Damage = 0.0f;

	UPROPERTY(EditAnywhere)
	float VelocityStrength = 20.0f;
};
