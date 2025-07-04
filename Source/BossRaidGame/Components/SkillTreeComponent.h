// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow ����
#include "SkillTreeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillAcquired);


class UAbilitySystemComponent;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSRAIDGAME_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USkillTreeComponent();

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool TryAcquireSkill(FName SkillID);

	/** Ư�� ��ų�� �̹� ������� Ȯ���մϴ�. */
	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool HasLearnedSkill(FName SkillID) const;


public:
	// ��ų ���� ���� �� ȣ��� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "Skill Tree")
	FOnSkillAcquired OnSkillAcquired;

	const FSkillTreeDataRow* GetSkillData(const FName& SkillID) const;
	const FSkillTreeDataRow* FindSkillDataByGrantedAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;

protected:
	// ������Ʈ�� ���۵� �� ȣ��˴ϴ�.
	virtual void BeginPlay() override;

	// ���� �Լ���
	

protected:
	// ======[ �� �κ��� �߰�/�����մϴ� ]======
	// ������ ��ų���� ID ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Tree")
	TArray<FName> AcquiredSkills;

	// ����� Skill Tree DataTable
	UPROPERTY(EditAnywhere, Category = "Skill Tree")
	UDataTable* SkillDataTable;



private:
	// ������(�÷��̾�)�� AbilitySystemComponent ĳ��
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	// ������(�÷��̾�)�� AttributeSet ĳ��
	UPROPERTY()
	TObjectPtr<class UPlayerCharacterAttributeSet> OwnerAttributeSet;
};
