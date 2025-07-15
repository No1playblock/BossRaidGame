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

	// �ֱ������� ������ �����ϱ� ���� ȣ��� �Լ��Դϴ�.
	UFUNCTION()
	void ApplyHealing();

	// �浹 �� ���� ������ ���� ���Ǿ� ������Ʈ�Դϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	// �ð� ȿ���� ���� ��ƼŬ �ý��� ������Ʈ�Դϴ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	// �������Ʈ���� ������, ������ ���� �����÷��� ����Ʈ Ŭ�����Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> HealingEffectClass;

	// ������ ����Ǵ� ���� (��) �Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float HealingInterval = 1.0f;

	// ���� ������ �� ���� (��) �Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	float PoolLifetime = 10.0f;

private:
	// ���� Ÿ�̸Ӹ� ���� �ڵ��Դϴ�.
	FTimerHandle HealingTimerHandle;

};
