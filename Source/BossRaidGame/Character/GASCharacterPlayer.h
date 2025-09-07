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
	
	// UMG ��ư�� ȣ���� ���� ��û �Լ�
	UFUNCTION(BlueprintCallable, Category = "GAS|Input")
	void RequestPrimaryAttackPressed();

	UFUNCTION(BlueprintCallable, Category = "GAS|Input")
	void RequestPrimaryAttackReleased();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USkillTreeComponent* GetSkillTreeComponent() const { return SkillTreeComponent; }

	FORCEINLINE AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor; }

	void SetInteractableActor(AActor* Interactable);
	void ClearInteractableActor(AActor* Interactable);

	void ActivateAbilityByInputID(int32 InputId);

	//BossCharacter�� ���� ��ų�� ���� �������� �ٸ��� �ϱ� ���� ������� �޼ҵ�
	//�÷��̾�� AttackPower
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
	// ���� ��ȣ�ۿ� ������ ���͸� ������ ����
	UPROPERTY()
	AActor* CurrentInteractableActor = nullptr;
};
