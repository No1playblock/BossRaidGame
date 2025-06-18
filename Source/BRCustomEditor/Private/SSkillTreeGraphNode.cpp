

#include "SSkillTreeGraphNode.h"
#include "SkillTreeEdGraphNode.h"
#include "Widgets/Text/STextBlock.h"
#include "SGraphPanel.h"

void SSkillTreeGraphNode::Construct(const FArguments& InArgs, USkillTreeEdGraphNode* InNode)
{
    GraphNode = InNode; // SGraphNode의 멤버 변수인 GraphNode에 우리 데이터 노드를 저장
    UpdateGraphNode();  // 노드 위젯의 내용을 채우는 함수 호출
}

void SSkillTreeGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// 핀들을 담을 좌/우 박스를 먼저 비워줍니다.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	// SNew를 사용해 이 노드의 시각적 모양을 새롭게 정의합니다.
	// 좌(입력핀)/중앙(콘텐츠)/우(출력핀) 3단 구조를 만듭니다.
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		[
			SNew(SHorizontalBox)

				// 1. 왼쪽: 입력 핀들이 들어갈 SVerticalBox를 생성하고 LeftNodeBox에 할당합니다.
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]

				// 2. 중앙: 노드의 주요 내용이 들어갑니다.
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							// 노드 제목
							SNew(STextBlock)
								.Text(this, &SSkillTreeGraphNode::GetTitle)
						]
						// (나중에 여기에 아이콘이나 설명을 추가할 수 있습니다)
				]

				// 3. 오른쪽: 출력 핀들이 들어갈 SVerticalBox를 생성하고 RightNodeBox에 할당합니다.
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
		];

	// 이제 LeftNodeBox와 RightNodeBox가 유효한 상태이므로, 핀 위젯들을 생성합니다.
	CreatePinWidgets();
}

FText SSkillTreeGraphNode::GetTitle() const
{
    // GraphNode를 우리의 커스텀 노드 타입으로 캐스팅합니다.
    if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode))
    {
        // 데이터 노드에 우리가 직접 만든 GetNodeTitle 함수를 호출합니다.
        return SkillNode->GetNodeTitle(ENodeTitleType::FullTitle);
    }
    return FText::GetEmpty();
}