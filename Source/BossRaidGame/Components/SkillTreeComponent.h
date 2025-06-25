// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow ����
#include "SkillTreeComponent.generated.h"

class UAbilitySystemComponent;
class ACharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSRAIDGAME_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USkillTreeComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	TArray<FName> AcquiredSkills;

	// ����� Skill Tree DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	UDataTable* SkillDataTable;

	// SkillID�� ������ �˻�
	const FSkillTreeDataRow* GetSkillData(const FName& SkillID) const;

	// �ο��� AbilityClass�� SkillData �˻�
	const FSkillTreeDataRow* FindSkillDataByGrantedAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;
};
