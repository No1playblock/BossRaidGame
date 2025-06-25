

#include "SSkillTreeGraphNode.h"
#include "SkillTreeEdGraphNode.h"
#include "Widgets/Text/STextBlock.h"
#include "SGraphPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h" // [�߰�] �ζ��� ���� ���� �ؽ�Ʈ ��� ���
#include "Widgets/Images/SImage.h" // SImage �߰�
#include "Widgets/Input/SNumericEntryBox.h" // SNumericEntryBox �߰�
#include "ScopedTransaction.h" // FScopedTransaction �߰�
#include "Styling/SlateBrush.h" // FSlateBrush


void SSkillTreeGraphNode::Construct(const FArguments& InArgs, USkillTreeEdGraphNode* InNode)
{
	GraphNode = InNode; // SGraphNode�� ��� ������ GraphNode�� �츮 ������ ��带 ����
	UpdateGraphNode();  // ��� ������ ������ ä��� �Լ� ȣ��


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

	// �ɵ��� ���� ��/�� �ڽ��� ���� ����ݴϴ�.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	// ������ ǥ�ø� ���� �귯�ø� �����ͷκ��� ������Ʈ�մϴ�.
	IconBrush = MakeShareable(new FSlateBrush());
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		if (UTexture2D* IconTexture = SkillNode->SkillData.SkillIcon.LoadSynchronous())
		{
			IconBrush->SetResourceObject(IconTexture);
			IconBrush->SetImageSize(FVector2D(48.f, 48.f)); // ������ ũ�� ����
		}
	}

	TSharedPtr<SVerticalBox> MainContentBox = SNew(SVerticalBox)

		// 1. ������ + Ÿ��Ʋ
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

		// 2. ����
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
		// 3. ���м�
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
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text(FText::FromString("Damage:"))
						]
						+ SHorizontalBox::Slot().AutoWidth().Padding(5, 0).VAlign(VAlign_Center)
						[
							SNew(SNumericEntryBox<float>)
								.Value(this, &SSkillTreeGraphNode::GetSkillDamage)
								.OnValueCommitted(this, &SSkillTreeGraphNode::OnSkillDamageCommitted)
						]
				]

				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text(FText::FromString("CoolTime:"))
						]
						+ SHorizontalBox::Slot().AutoWidth().Padding(5, 0).VAlign(VAlign_Center)
						[
							SNew(SNumericEntryBox<float>)
								.Value(this, &SSkillTreeGraphNode::GetSkillCoolTime)
								.OnValueCommitted(this, &SSkillTreeGraphNode::OnSkillCoolTimeCommitted)
						]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text(FText::FromString("Skill Point:"))
						]
						+ SHorizontalBox::Slot().AutoWidth().Padding(5, 0).VAlign(VAlign_Center)
						[
							SNew(SNumericEntryBox<int32>)
								.Value(this, &SSkillTreeGraphNode::GetSkillPointCost)
								.OnValueCommitted(this, &SSkillTreeGraphNode::OnSkillPointCostCommitted)
						]
				]

		];
	

	// ���� ��� �ܰ� ����
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
				.BorderBackgroundColor(FLinearColor(0.001f, 0.001f, 0.001f, 1.0f)) // �ʰ� ������ ����
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


// --- Getter �Լ��� ---

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

TOptional<float> SSkillTreeGraphNode::GetSkillCoolTime() const
{
	if (USkillTreeEdGraphNode* Node = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		return Node->SkillData.SkillCoolTime;
	}
	return TOptional<float>();
}


// --- Committer �Լ��� (���� �� ������ ���� �� Undo/Redo ���) ---

void SSkillTreeGraphNode::OnTitleCommitted(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		if (SkillNode->SkillData.SkillName.EqualTo(InText))
		{
			return; // ������� ����
		}

		const FScopedTransaction Transaction(FText::FromString(TEXT("Edit Skill Name")));
		SkillNode->Modify();
		UE_LOG(LogTemp, Warning, TEXT("GraphNode Class: %s"), *GraphNode->GetClass()->GetName());

		UE_LOG(LogTemp, Warning, TEXT("MODIFY"));
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

void SSkillTreeGraphNode::OnSkillCoolTimeCommitted(float NewValue, ETextCommit::Type CommitType)
{
	if (USkillTreeEdGraphNode* Node = Cast<USkillTreeEdGraphNode>(GraphNode))
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Change Skill CoolTime")));
		Node->Modify();
		Node->SkillData.SkillCoolTime = NewValue;
	}
}
