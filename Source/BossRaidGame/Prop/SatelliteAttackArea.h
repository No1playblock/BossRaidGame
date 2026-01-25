// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h" 
#include "GameplayEffect.h"
#include "Interface/SkillSpawnedActorInterface.h" // 헤더 포함
#include "SatelliteAttackArea.generated.h"

UCLASS()
class BOSSRAIDGAME_API ASatelliteAttackArea : public AActor, public IPoolableInterface, public ISkillSpawnedActorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASatelliteAttackArea();

protected:
	// 풀에서 꺼내질 때 호출
	virtual void OnPoolSpawned() override;
	virtual void OnPoolDespawned() override;

public:
	
	virtual void InitializeInfo_Implementation(float Damage, TSubclassOf<UGameplayEffect> DamageEffect) override;
	virtual void TriggerSkill_Implementation() override;

protected:

	// 풀로 돌아가는 함수
	void ReturnPool();

	void StartAiming();
	void ExecuteExplosion();


protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> RootComp;

	// 범위 확인용 콜리전 (실제 충돌은 안 하고 시각적/디버그 용도 혹은 Overlap 쿼리 기준점)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UParticleSystemComponent> AimingParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> ExplosionVFX;	

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UParticleSystem> AimingVFX;

	// 효과음
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<USoundBase> ExplosionSound;

	// 전달받은 데이터 저장용

	UPROPERTY(EditAnywhere)
	float AttackRange;

	float AttackRadius;
	float DamageAmount;
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 스스로 반환하기 위한 타이머 핸들
	FTimerHandle ReturnTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float AimingVFXScale = 2.75f;


	UPROPERTY(EditAnywhere, Category = "Skill")
	float ExplosionScale = 2.75f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SphereRadius = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	bool DrawDebug = true;
};
