#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "SkillTreeEdGraphNode.h"
#include "SSkillTreeGraphNode.h"

class FSkillTreeGraphPanelNodeFactory : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        // 만약 들어온 노드가 USkillTreeEdGraphNode 타입이라면
        if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(Node))
        {
            // SSkillTreeGraphNode 위젯을 생성해서 반환
            return SNew(SSkillTreeGraphNode, SkillNode);
        }
        // 아니라면 null을 반환하여 다른 팩토리가 처리
        return nullptr;
    }
};