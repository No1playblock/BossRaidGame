// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h" // 이 줄을 추가해주세요.
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	FVector ArcCenterOffset = FVector(150.0f, 0.0f, 0.0f); // 호의 중심 위치 오프셋

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float ArcRadius = 150.0f; // 호의 반지름

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float SphereRadius = 50.0f; // 개별 구체의 반지름

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	int32 SphereNum = 5; // 생성할 구체의 개수

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float ArcStartYaw = -45.0f; // 호의 시작 각도 (캐릭터 정면 기준)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	bool bIsClockwise = true; // 시계방향 여부

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Arc Sweep")
	float DelayBetweenSpheres = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TEnumAsByte<ECollisionChannel> TargetChannel;

private:
	FDelegateHandle CancelledHandle;
	FDelegateHandle EndedHandle;
};
