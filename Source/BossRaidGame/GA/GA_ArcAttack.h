// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h" // �� ���� �߰����ּ���.
#include "Engine/EngineTypes.h"
#include "GA_ArcAttack.generated.h"

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API UGA_ArcAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ArcAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:

	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetsHit(const TArray<FOverlapResult>& OverlapResults);

	UFUNCTION()
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	// �� �����Ƽ�� ��ٿ��� ��Ÿ���� �±� (HUD�� �����ϱ� ����)
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

	// �ִϸ��̼ǿ��� ������ ������ �Ͼ�� ������ �˷��� �±�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FGameplayTag DamageEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	FVector ArcCenterOffset = FVector(150.0f, 0.0f, 0.0f); // ȣ�� �߽� ��ġ ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float ArcRadius = 150.0f; // ȣ�� ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float SphereRadius = 50.0f; // ���� ��ü�� ������

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	int32 SphereNum = 5; // ������ ��ü�� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float ArcStartYaw = -45.0f; // ȣ�� ���� ���� (ĳ���� ���� ����)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	bool bIsClockwise = true; // �ð���� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float DelayBetweenSpheres = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;

private:
	FDelegateHandle CancelledHandle;
	FDelegateHandle EndedHandle;
};
