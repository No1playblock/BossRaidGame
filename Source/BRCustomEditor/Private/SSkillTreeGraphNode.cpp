

#include "SSkillTreeGraphNode.h"
#include "SkillTreeEdGraphNode.h"
#include "Widgets/Text/STextBlock.h"
#include "SGraphPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h" // [추가] 인라인 편집 가능 텍스트 블록 헤더
#include "Widgets/Images/SImage.h" // SImage 추가
#include "Widgets/Input/SNumericEntryBox.h" // SNumericEntryBox 추가
#include "ScopedTransaction.h" // FScopedTransaction 추가
#include "Styling/SlateBrush.h" // FSlateBrush

void SSkillTreeGraphNode::Construct(const FArguments& InArgs, USkillTreeEdGraphNode* InNode)
{
    GraphNode = InNode; // SGraphNode의 멤버 변수인 GraphNode에 우리 데이터 노드를 저장
    UpdateGraphNode();  // 노드 위젯의 내용을 채우는 함수 호출


}

void SSkillTreeGraphNode::UpdateGraphNode()
{
	const FSlateFontInfo TitleFont = FSlateFontInfo(
		FPaths::ProjectContentDir() / TEXT("Assets/Fonts/SB_Agro.ttf"),
		18
	);

	const FSlateFontInfo DescriptionFont = FSlateFontInfo(
		FPaths::ProjectContentDir() / TEXT("Assets/Fonts/SB_Agro.ttf"),
		12
	);

	InputPins.Empty();
	OutputPins.Empty();

	// 핀들을 담을 좌/우 박스를 먼저 비워줍니다.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	// 아이콘 표시를 위한 브러시를 데이터로부터 업데이트합니다.
	IconBrush = MakeShareable(new FSlateBrush());
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		if (UTexture2D* IconTexture = SkillNode->SkillData.SkillIcon.LoadSynchronous())
		{
			IconBrush->SetResourceObject(IconTexture);
			IconBrush->SetImageSize(FVector2D(48.f, 48.f)); // 아이콘 크기 지정
		}
	}

	TSharedPtr<SVerticalBox> MainContentBox = SNew(SVerticalBox)

		// 1. 아이콘 + 타이틀
		+ SVerticalBox::Slot().AutoHeight().Padding(8)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
						.BorderBackgroundColor(FLinearColor::White)
						.Padding(2)
						[
							SNew(SImage).Image(GetIconBrush())
						]
				]
				+ SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center).Padding(10, 0)
				[
					SNew(SInlineEditableTextBlock)
						.Text(this, &SSkillTreeGraphNode::GetTitle)
						.OnTextCommitted(this, &SSkillTreeGraphNode::OnTitleCommitted)
						.IsReadOnly(false)
						.Font(TitleFont)
						.ColorAndOpacity(FLinearColor::White)
				]
		]

		// 2. 설명
		+ SVerticalBox::Slot().AutoHeight().Padding(8)
		[
			SNew(SInlineEditableTextBlock)
				.Text(this, &SSkillTreeGraphNode::GetDescription)
				.OnTextCommitted(this, &SSkillTreeGraphNode::OnDescriptionCommitted)
				.IsReadOnly(false)
				.MultiLine(true)
				.WrapTextAt(240.f)
				.Font(DescriptionFont)
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
		]
		// 3. 구분선
		+ SVerticalBox::Slot().AutoHeight().Padding(5)
		[
			SNew(SBox).HeightOverride(1.0f)
				[
					SNew(SImage).ColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f))
				]
		]

		// 4. Cost & Damage
		+ SVerticalBox::Slot().AutoHeight().Padding(8)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("Cost:")).ColorAndOpacity(FLinearColor::White)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(10, 0).VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<int32>)
						.Value(this, &SSkillTreeGraphNode::GetSkillPointCost)
						.OnValueCommitted(this, &SSkillTreeGraphNode::OnSkillPointCostCommitted)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(20, 0).VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("Damage:")).ColorAndOpacity(FLinearColor::White)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(10, 0).VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<float>)
						.Value(this, &SSkillTreeGraphNode::GetSkillDamage)
						.OnValueCommitted(this, &SSkillTreeGraphNode::OnSkillDamageCommitted)
				]
		];

	// 최종 노드 외곽 구성
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
				.BorderBackgroundColor(FLinearColor(0.001f, 0.001f, 0.001f, 1.0f)) // 너가 지정한 배경색
				.Padding(0)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth()[SAssignNew(LeftNodeBox, SVerticalBox)]
						+ SHorizontalBox::Slot().FillWidth(1.0f)[MainContentBox.ToSharedRef()]
						+ SHorizontalBox::Slot().AutoWidth()[SAssignNew(RightNodeBox, SVerticalBox)]
				]
		];

	CreatePinWidgets();
}


// --- Getter 함수들 ---

FText SSkillTreeGraphNode::GetTitle() const
{
	if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode)) { return SkillNode->SkillData.SkillName; }
	return FText::GetEmpty();
}

FText SSkillTreeGraphNode::GetDescription() const
{
	if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode)) { return SkillNode->SkillData.UpgradeDescription; }
	return FText::GetEmpty();
}

const FSlateBrush* SSkillTreeGraphNode::GetIconBrush() const
{
	return IconBrush.Get();
}

TOptional<int32> SSkillTreeGraphNode::GetSkillPointCost() const
{
	if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode)) { return SkillNode->SkillData.SkillPointCost; }
	return 0;
}

TOptional<float> SSkillTreeGraphNode::GetSkillDamage() const
{
	if (const USkillTreeEdGraphNode* SkillNode = Cast<const USkillTreeEdGraphNode>(GraphNode)) { return SkillNode->SkillData.SkillDamage; }
	return 0.f;
}


// --- Committer 함수들 (수정 시 데이터 변경 및 Undo/Redo 등록) ---

void SSkillTreeGraphNode::OnTitleCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Skill Name")));
		SkillNode->Modify();
		SkillNode->SkillData.SkillName = InText;
		SkillNode->GetGraph()->NotifyGraphChanged();
	}
}

void SSkillTreeGraphNode::OnDescriptionCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Skill Description")));
		SkillNode->Modify();
		SkillNode->SkillData.UpgradeDescription = InText;
		SkillNode->GetGraph()->NotifyGraphChanged();
	}
}

void SSkillTreeGraphNode::OnSkillPointCostCommitted(int32 InValue, ETextCommit::Type CommitInfo)
{
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Skill Point Cost")));
		SkillNode->Modify();
		SkillNode->SkillData.SkillPointCost = InValue;
		SkillNode->GetGraph()->NotifyGraphChanged();
	}
}

void SSkillTreeGraphNode::OnSkillDamageCommitted(float InValue, ETextCommit::Type CommitInfo)
{
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Skill Damage")));
		SkillNode->Modify();
		SkillNode->SkillData.SkillDamage = InValue;
		SkillNode->GetGraph()->NotifyGraphChanged();
	}
}