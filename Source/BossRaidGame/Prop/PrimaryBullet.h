// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "PrimaryBullet.generated.h"


UCLASS()
class BOSSRAIDGAME_API APrimaryBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APrimaryBullet();

	void InitializeBullet(float InDamage, TSubclassOf<class UGameplayEffect> InDamageEffectClass);
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> BulletMesh;

	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	float Damage = 0.0f;
};
