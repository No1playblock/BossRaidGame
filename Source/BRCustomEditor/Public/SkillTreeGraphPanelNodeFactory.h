#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "SkillTreeEdGraphNode.h"
#include "SSkillTreeGraphNode.h"

class FSkillTreeGraphPanelNodeFactory : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        // ���� ���� ��尡 �츮�� ���� USkillTreeEdGraphNode Ÿ���̶��
        if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(Node))
        {
            // �츮�� ���� SSkillTreeGraphNode ������ �����ؼ� ��ȯ�մϴ�.
            return SNew(SSkillTreeGraphNode, SkillNode);
        }
        // �ƴ϶�� null�� ��ȯ�Ͽ� �ٸ� ���丮�� ó���ϵ��� �մϴ�.
        return nullptr;
    }
};