#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "SkillTreeEdGraphNode.h"
#include "SSkillTreeGraphNode.h"

class FSkillTreeGraphPanelNodeFactory : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        // ���� ���� ��尡 USkillTreeEdGraphNode Ÿ���̶��
        if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(Node))
        {
            // SSkillTreeGraphNode ������ �����ؼ� ��ȯ
            return SNew(SSkillTreeGraphNode, SkillNode);
        }
        // �ƴ϶�� null�� ��ȯ�Ͽ� �ٸ� ���丮�� ó��
        return nullptr;
    }
};