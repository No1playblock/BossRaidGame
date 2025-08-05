// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h" // �� ���� �߰����ּ���.
#include "Engine/EngineTypes.h"
#include "GA_BaseMeleeAttack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BOSSRAIDGAME_API UGA_BaseMeleeAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_BaseMeleeAttack();

	// �����Ƽ�� Ȱ��ȭ�� �� ���� ���� ȣ��˴ϴ�.
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);


protected:

	UFUNCTION()
	void OnDamageEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnTargetsHit(const TArray<AActor*>& HitActors);

	UFUNCTION()
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	//// ��ٿ��� ������ �����÷��� ����Ʈ
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	//TSubclassOf<UGameplayEffect> CooldownEffectClass;

	// �ִϸ��̼ǿ��� ������ ������ �Ͼ�� ������ �˷��� �±�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	FGameplayTag DamageEventTag;

	// ���� ���� ���� (������)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	float AttackRadius = 50.0f;

	UPROPERTY(EditAnyWhere)
	FVector AttackLocationOffset = FVector(0.0f, 0.0f, 50.0f); // ���� ��ġ ������

	UPROPERTY(EditAnyWhere, Category = "Melee Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;
private:
	FDelegateHandle CancelledHandle;
	FDelegateHandle EndedHandle;

	//TSet<TObjectPtr<AActor>> HitActors;
};
