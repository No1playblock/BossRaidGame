

#include "SSkillTreeGraphNode.h"
#include "SkillTreeEdGraphNode.h"
#include "Widgets/Text/STextBlock.h"
#include "SGraphPanel.h"

void SSkillTreeGraphNode::Construct(const FArguments& InArgs, USkillTreeEdGraphNode* InNode)
{
    GraphNode = InNode; // SGraphNode�� ��� ������ GraphNode�� �츮 ������ ��带 ����
    UpdateGraphNode();  // ��� ������ ������ ä��� �Լ� ȣ��
}

void SSkillTreeGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// �ɵ��� ���� ��/�� �ڽ��� ���� ����ݴϴ�.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	// SNew�� ����� �� ����� �ð��� ����� ���Ӱ� �����մϴ�.
	// ��(�Է���)/�߾�(������)/��(�����) 3�� ������ ����ϴ�.
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		[
			SNew(SHorizontalBox)

				// 1. ����: �Է� �ɵ��� �� SVerticalBox�� �����ϰ� LeftNodeBox�� �Ҵ��մϴ�.
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]

				// 2. �߾�: ����� �ֿ� ������ ���ϴ�.
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							// ��� ����
							SNew(STextBlock)
								.Text(this, &SSkillTreeGraphNode::GetTitle)
						]
						// (���߿� ���⿡ �������̳� ������ �߰��� �� �ֽ��ϴ�)
				]

				// 3. ������: ��� �ɵ��� �� SVerticalBox�� �����ϰ� RightNodeBox�� �Ҵ��մϴ�.
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
		];

	// ���� LeftNodeBox�� RightNodeBox�� ��ȿ�� �����̹Ƿ�, �� �������� �����մϴ�.
	CreatePinWidgets();
}

FText SSkillTreeGraphNode::GetTitle() const
{
    // GraphNode�� �츮�� Ŀ���� ��� Ÿ������ ĳ�����մϴ�.
    if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode))
    {
        // ������ ��忡 �츮�� ���� ���� GetNodeTitle �Լ��� ȣ���մϴ�.
        return SkillNode->GetNodeTitle(ENodeTitleType::FullTitle);
    }
    return FText::GetEmpty();
}