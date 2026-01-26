// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/SkillTreeData.h" 
#include "SkillTreeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSkillAcquired);


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

	//특정 스킬을 이미 배웠는지 확인
	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool HasLearnedSkill(FName SkillID) const;


public:
	// 스킬 습득 성공 시 호출될 델리게이트
	FOnSkillAcquired OnSkillAcquired;

	const FSkillTreeDataRow* GetSkillData(const FName& SkillID) const;
	const FSkillTreeDataRow* FindSkillDataByGrantedAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;
	const FSkillTreeDataRow* FindAcquiredSkillByInputID(EAbilityInputID InputID) const;

protected:

	virtual void BeginPlay() override;


protected:
	
	// 습득한 스킬들의 ID 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Tree")
	TArray<FName> AcquiredSkills;

	// 연결된 Skill Tree DataTable
	UPROPERTY(EditAnywhere, Category = "Skill Tree")
	UDataTable* SkillDataTable;



private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	UPROPERTY()
	TObjectPtr<class UPlayerCharacterAttributeSet> OwnerAttributeSet;
};
