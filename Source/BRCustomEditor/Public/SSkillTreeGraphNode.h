#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SSkillTreeGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SSkillTreeGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, class USkillTreeEdGraphNode* InNode);

    // ����� ������ �����ϰ� ������Ʈ�ϴ� �ٽ� �Լ��Դϴ�.
    virtual void UpdateGraphNode() override;

private:
    // [�߰�] ��� ������ �������� ���� ���� �Լ�
    FText GetTitle() const;
};