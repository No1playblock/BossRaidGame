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
	FText GetDescription() const;
	const FSlateBrush* GetIconBrush() const;
	TOptional<int32> GetSkillPointCost() const;
	TOptional<float> GetSkillDamage() const;

	// --- Commit Handlers (UI���� ���� ������ �� ȣ��� �Լ���) ---
	void OnTitleCommitted(const FText& InText, ETextCommit::Type CommitInfo);
	void OnDescriptionCommitted(const FText& InText, ETextCommit::Type CommitInfo);
	void OnSkillPointCostCommitted(int32 InValue, ETextCommit::Type CommitInfo);
	void OnSkillDamageCommitted(float InValue, ETextCommit::Type CommitInfo);

	// �������� ǥ���ϱ� ���� Slate �귯��
	TSharedPtr<FSlateBrush> IconBrush;

};