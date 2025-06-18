// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

struct FSkillTreeDataRow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSRAIDGAME_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USkillComponent();

	// 스킬 업그레이드를 시도하는 메인 함수
	UFUNCTION(BlueprintCallable, Category = "SkillTree")
	void UpgradeSkill(const FName& TargetSkillID);

protected:
	virtual void BeginPlay() override;

private:
	// 스킬 데이터 테이블을 로드하기 위한 변수
	UPROPERTY()
	TObjectPtr<class UDataTable> SkillTreeDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "SkillTree")
	TSoftObjectPtr<UDataTable> SkillTreeDataTablePath;

	// 컴포넌트의 소유주(플레이어)와 ASC에 대한 약참조 포인터
	UPROPERTY()
	TObjectPtr<class ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> OwnerASC;
		
};
