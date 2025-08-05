#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class SSkillTreeGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SSkillTreeGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, class USkillTreeEdGraphNode* InNode);

    virtual void UpdateGraphNode() override;

private:
	FText GetTitle() const;
	FText GetDescription() const;
	const FSlateBrush* GetIconBrush() const;
	TOptional<int32> GetSkillPointCost() const;
	TOptional<float> GetSkillDamage() const;
	TOptional<float> GetSkillCoolTime() const;

	void OnTitleCommitted(const FText& InText, ETextCommit::Type CommitInfo);
	void OnDescriptionCommitted(const FText& InText, ETextCommit::Type CommitInfo);
	void OnSkillPointCostCommitted(int32 InValue, ETextCommit::Type CommitInfo);
	void OnSkillDamageCommitted(float InValue, ETextCommit::Type CommitInfo);

	void OnSkillCoolTimeCommitted(float NewValue, ETextCommit::Type CommitType);

	// 아이콘을 표시하기 위한 Slate 브러시
	TSharedPtr<FSlateBrush> IconBrush;

};