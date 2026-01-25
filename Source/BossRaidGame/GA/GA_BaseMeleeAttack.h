// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h" // 이 줄을 추가해주세요.
#include "Engine/EngineTypes.h"
#include "GameData/SkillTreeData.h"
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	// 이 어빌리티의 쿨다운을 나타내는 태그 (HUD와 연동하기 위함)
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

private:
	FDelegateHandle CancelledHandle;
	FDelegateHandle EndedHandle;

	float Damage;

};
