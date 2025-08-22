// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h" 
#include "Interface/DamageDataProvider.h"
#include "NonPlayerGASCharacter.generated.h"

/**
 * 
 */
class AMobSpawnManager;
UCLASS()
class BOSSRAIDGAME_API ANonPlayerGASCharacter : public ABaseCharacter, public IAbilitySystemInterface, public IDamageDataProvider
{
	GENERATED_BODY()
	
public:
	ANonPlayerGASCharacter();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	// 캐릭터가 풀에서 활성화될 때 호출할 함수
	void ActivateCharacter();
	// 이 캐릭터가 풀로 돌아갈 때 호출할 함수
	void DeactivateCharacter();

	void SetOwningSpawnManager(AMobSpawnManager* InManager);

	virtual void OnOutOfHealth() override;

	//BossCharacter는 스킬에 따라 데미지가 달라 만들어진 메소드
	virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const override;


	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> GainExpEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UMobCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = Pawn)
	TObjectPtr<class UBehaviorTree> BehaviorTree;
	
	TWeakObjectPtr<AMobSpawnManager> OwningSpawnManager;

	UPROPERTY()
	TObjectPtr<AController> MyAIController;


};
