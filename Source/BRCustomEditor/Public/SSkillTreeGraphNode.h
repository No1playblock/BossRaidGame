#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SSkillTreeGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SSkillTreeGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, class USkillTreeEdGraphNode* InNode);

    // 노드의 내용을 구성하고 업데이트하는 핵심 함수입니다.
    virtual void UpdateGraphNode() override;

private:
    // [추가] 노드 제목을 가져오기 위한 헬퍼 함수
    FText GetTitle() const;
};