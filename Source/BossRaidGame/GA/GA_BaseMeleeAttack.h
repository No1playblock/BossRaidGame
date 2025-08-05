// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h" // 이 줄을 추가해주세요.
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

	// 어빌리티가 활성화될 때 가장 먼저 호출됩니다.
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

	//// 쿨다운을 적용할 게임플레이 이펙트
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	//TSubclassOf<UGameplayEffect> CooldownEffectClass;

	// 애니메이션에서 데미지 판정이 일어나는 시점을 알려줄 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	FGameplayTag DamageEventTag;

	// 공격 판정 범위 (반지름)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	float AttackRadius = 50.0f;

	UPROPERTY(EditAnyWhere)
	FVector AttackLocationOffset = FVector(0.0f, 0.0f, 50.0f); // 공격 위치 오프셋

	UPROPERTY(EditAnyWhere, Category = "Melee Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;
private:
	FDelegateHandle CancelledHandle;
	FDelegateHandle EndedHandle;

	//TSet<TObjectPtr<AActor>> HitActors;
};
