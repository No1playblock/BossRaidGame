// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeNodeWidget.generated.h"


class UImage;
class UTextBlock;
class UBorder;
class UButton;
UCLASS()
class BOSSRAIDGAME_API USkillTreeNodeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ��ų �����͸� �����ϰ� UI�� ������Ʈ�մϴ�.
	void SetSkillData(const FSkillTreeDataRow& InData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FDataTableRowHandle SkillRowHandle;

	// �ܺο��� �� ����� ��ų �����͸� ������ �� �ֵ��� Getter�� �����մϴ�.
	FORCEINLINE const FSkillTreeDataRow* GetNodeData() const { return CachedSkillData; }

protected:

	virtual void NativePreConstruct() override;

	void UpdateNodeVisuals();


	// �������Ʈ ������ ������Ʈ�� ���ε��� ������
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> NodeBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SkillButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillDamageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillCoolTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillCostText;

	// ���� ����� ��ų ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Node")
	FSkillTreeDataRow SkillData;

	FSkillTreeDataRow* CachedSkillData;

	// SkillRowHandle�� ��� ������ �������� ���� UI�� ������Ʈ�ϴ� �Լ�

};
