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
    // 이 노드가 담고 있는 실제 스킬 데이터
    UPROPERTY(EditAnywhere, Category = "SkillTree")
    FSkillTreeDataRow SkillData;

    // 그래프 에디터에서 노드의 제목을 설정하는 함수
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;


    virtual void AllocateDefaultPins() override;

    UEdGraphPin* GetInputPin() const;
    UEdGraphPin* GetOutputPin() const;
};
