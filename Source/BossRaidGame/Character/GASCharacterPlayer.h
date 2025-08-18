// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "CharacterPlayer.h"
#include "Interface/DamageDataProvider.h"
#include "GASCharacterPlayer.generated.h"


class UDataTable;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BOSSRAIDGAME_API AGASCharacterPlayer : public ACharacterPlayer, public IAbilitySystemInterface, public IDamageDataProvider
{
	GENERATED_BODY()
	
public:

	AGASCharacterPlayer();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USkillTreeComponent* GetSkillTreeComponent() const { return SkillTreeComponent; }

	FORCEINLINE AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor; }

	void SetInteractableActor(AActor* Interactable);
	void ClearInteractableActor(AActor* Interactable);

	//BossCharacter를 위해 스킬에 따라 데미지를 다르게 하기 위해 만들어진 메소드
	//플레이어는 AttackPower
	virtual float GetDamageByAttackTag(const FGameplayTag& AttackTag) const override;

protected:

	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPlayerCharacterAttributeSet> AttributeSet;

	FDelegateHandle MoveSpeedChangedDelegateHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Tree", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillTreeComponent> SkillTreeComponent;

private:
	// 현재 상호작용 가능한 액터를 저장할 변수
	UPROPERTY()
	AActor* CurrentInteractableActor = nullptr;
};
