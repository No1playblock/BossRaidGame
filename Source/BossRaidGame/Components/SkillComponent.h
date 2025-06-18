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

	// ��ų ���׷��̵带 �õ��ϴ� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "SkillTree")
	void UpgradeSkill(const FName& TargetSkillID);

protected:
	virtual void BeginPlay() override;

private:
	// ��ų ������ ���̺��� �ε��ϱ� ���� ����
	UPROPERTY()
	TObjectPtr<class UDataTable> SkillTreeDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "SkillTree")
	TSoftObjectPtr<UDataTable> SkillTreeDataTablePath;

	// ������Ʈ�� ������(�÷��̾�)�� ASC�� ���� ������ ������
	UPROPERTY()
	TObjectPtr<class ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> OwnerASC;
		
};
