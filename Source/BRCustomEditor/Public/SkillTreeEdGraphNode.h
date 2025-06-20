// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "GameData/SkillTreeData.h"
#include "SkillTreeEdGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class BRCUSTOMEDITOR_API USkillTreeEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
    // �� ��尡 ��� �ִ� ���� ��ų ������
    UPROPERTY(EditAnywhere, Category = "SkillTree")
    FSkillTreeDataRow SkillData;

    // �׷��� �����Ϳ��� ����� ������ �����ϴ� �Լ�
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;


    virtual void AllocateDefaultPins() override;

    UEdGraphPin* GetInputPin() const;
    UEdGraphPin* GetOutputPin() const;
};
