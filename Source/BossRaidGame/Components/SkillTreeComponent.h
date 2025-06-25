// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/SkillTreeData.h" // FSkillTreeDataRow 포함
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

	// 연결된 Skill Tree DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	UDataTable* SkillDataTable;

	// SkillID로 데이터 검색
	const FSkillTreeDataRow* GetSkillData(const FName& SkillID) const;

	// 부여된 AbilityClass로 SkillData 검색
	const FSkillTreeDataRow* FindSkillDataByGrantedAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;
};
