// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow 포함
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

	/** 특정 스킬을 이미 배웠는지 확인합니다. */
	UFUNCTION(BlueprintPure, Category = "Skill Tree")
	bool HasLearnedSkill(FName SkillID) const;


public:
	// 스킬 습득 성공 시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Skill Tree")
	FOnSkillAcquired OnSkillAcquired;

	const FSkillTreeDataRow* GetSkillData(const FName& SkillID) const;
	const FSkillTreeDataRow* FindSkillDataByGrantedAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;

protected:
	// 컴포넌트가 시작될 때 호출됩니다.
	virtual void BeginPlay() override;

	// 기존 함수들
	

protected:
	// ======[ 이 부분을 추가/수정합니다 ]======
	// 습득한 스킬들의 ID 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Tree")
	TArray<FName> AcquiredSkills;

	// 연결된 Skill Tree DataTable
	UPROPERTY(EditAnywhere, Category = "Skill Tree")
	UDataTable* SkillDataTable;



private:
	// 소유자(플레이어)의 AbilitySystemComponent 캐시
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	// 소유자(플레이어)의 AttributeSet 캐시
	UPROPERTY()
	TObjectPtr<class UPlayerCharacterAttributeSet> OwnerAttributeSet;
};
