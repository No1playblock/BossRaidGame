// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AutoRaser.generated.h"

/**
 *
 */
UCLASS()
class BOSSRAIDGAME_API UGA_AutoRaser : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AutoRaser();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	void SpawnOrbsAndFire(ACharacter* OwnerCharacter, float Damage);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<class ARaserOrb> OrbActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float OrbSpawnRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float FireDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnyWhere, Category = "Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;

	// �� �����Ƽ�� ��ٿ��� ��Ÿ���� �±� (HUD�� �����ϱ� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditAnywhere)
	float OrbCount;

};
