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

	// ĳ���Ͱ� Ǯ���� Ȱ��ȭ�� �� ȣ���� �Լ�
	void ActivateCharacter();
	// �� ĳ���Ͱ� Ǯ�� ���ư� �� ȣ���� �Լ�
	void DeactivateCharacter();

	void SetOwningSpawnManager(AMobSpawnManager* InManager);

	virtual void OnOutOfHealth() override;

	//BossCharacter�� ��ų�� ���� �������� �޶� ������� �޼ҵ�
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

	/*NavMesh üũ�� ���� ������*/
	FTimerHandle AIStartTimerHandle;

	// NavMesh �غ� ���¸� �ֱ������� Ȯ���ϰ� AI�� �����ϴ� �Լ�
	void CheckNavMeshAndStartAI();
};
