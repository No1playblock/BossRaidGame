// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h" // [�߰�] ��� ���� ���
#include "EdGraph/EdGraph.h" // BreakPinLinks�� ���� �߰�
#include "Framework/Commands/GenericCommands.h" // FGraphContextMenuBuilder�� ���� �ʿ��� �� ����
#include "SkillTreeEdGraphNode.h" // USkillTreeEdGraphNode�� ����ϱ� ���� �߰�

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UE_LOG(LogTemp, Warning, TEXT("CanCreateConnection"));
	// 1. ���� ��忡 �����ϴ� ���� �����մϴ�.
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SamePin"));
	}

	// 2. ���� ������ �ɳ��� �����ϴ� ���� �����մϴ�.
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SameDirectionPin"));
	}

	// [���� �ڵ��� ������]
	// A �Ǵ� B�� nullptr�� ��츦 ������� �ʾ� �巡�� ������ �źεǾ����ϴ�.
	// ������ �� �Լ��� �巡�׸� ������ ���� B=nullptr ���·� ȣ��� �� �ֽ��ϴ�.

	// 3. ���� ī�װ��� �ٸ��� ������ �����ϴ�.
	if (A->PinType.PinCategory != B->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect AnotherType Pin"));
	}

	// 4. ��� ��Ģ�� ����ߴٸ� ������ ����մϴ�.
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connect"));
}

bool USkillTreeEdGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{

	const bool bMadeConnection = Super::TryCreateConnection(A, B);

	if (bMadeConnection)
	{
		// ���ῡ �����ߴٸ�, ������ ������Ʈ ������ �߰��մϴ�.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;

		USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
		USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

		if (SourceNode && TargetNode)
		{
			// �ҽ� ��尡 �б����� �ƴ϶�� NextLevelSkillID�� ������Ʈ�մϴ�.
			if (!SourceNode->SkillData.bIsBranchingPoint)
			{
				SourceNode->SkillData.NextLevelSkillID = TargetNode->SkillData.SkillID;
			}
			// �ҽ� ��尡 �б����̶�� BranchChoices �迭�� �߰��մϴ�.
			else
			{
				SourceNode->SkillData.BranchChoices.AddUnique(TargetNode->SkillData.SkillID);
			}

			// �׷����� ����Ǿ����� �����Ϳ� �˸��ϴ�.
			SourceNode->GetGraph()->NotifyGraphChanged();
		}
	}

	return bMadeConnection;

}

// [�߰�] CreateConnectionDrawingPolicy �Լ� ����
FConnectionDrawingPolicy* USkillTreeEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	
	//UE_LOG(LogTemp, Warning, TEXT("CreateConnectionDrawingPolicy"));

	return new FSkillTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void USkillTreeEdGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	// �θ� Ŭ������ ����� �״�� ȣ���Ͽ� �⺻���� ���� ���� ������ �����ϴ�.
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void USkillTreeEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText NodeCategory = FText::FromString(TEXT("Skill Tree"));
	const FText MenuDesc = FText::FromString(TEXT("Add New Skill Node"));
	const FText ToolTip = FText::FromString(TEXT("Add a new skill node to the graph."));

	TSharedPtr<FSkillTreeSchemaAction_NewNode> NewNodeAction = MakeShareable(new FSkillTreeSchemaAction_NewNode(NodeCategory, MenuDesc, ToolTip, 0));

	// �ɿ��� �巡���Ͽ� �޴��� �� ��� (����� �������)
	if (ContextMenuBuilder.FromPin)
	{
		ContextMenuBuilder.AddAction(NewNodeAction);
	}
	// �� �������� �޴��� �� ���
	else
	{
		ContextMenuBuilder.AddAction(NewNodeAction);
	}

	Super::GetGraphContextActions(ContextMenuBuilder);
}

void USkillTreeEdGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("Break Skill Link")));

	Super::BreakSinglePinLink(SourcePin, TargetPin);

	USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(SourcePin->GetOwningNode());
	USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(TargetPin->GetOwningNode());

	if (SourceNode && TargetNode)
	{
		SourceNode->Modify(); // [�߰�] SourceNode�� ���¸� ������ ���̶�� ������ �˸��ϴ�.

		// �ҽ� ��尡 �б����� �ƴ϶�� NextLevelSkillID�� ���ϴ�.
		if (!SourceNode->SkillData.bIsBranchingPoint)
		{
			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
			{
				SourceNode->SkillData.NextLevelSkillID = NAME_None;
			}
		}
		// �ҽ� ��尡 �б����̶�� BranchChoices �迭���� �ش� ID�� �����մϴ�.
		else
		{
			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
		}

		// �׷����� ����Ǿ����� �����Ϳ� �˸��ϴ�.
		SourceNode->GetGraph()->NotifyGraphChanged();
	}
}

USkillTreeEdGraphNode* USkillTreeEdGraphSchema::CreateNewSkillNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("Add New Skill Node")));
	ParentGraph->Modify();

	USkillTreeEdGraphNode* NewNode = NewObject<USkillTreeEdGraphNode>(ParentGraph, NAME_None, RF_Transactional);
	NewNode->NodeGuid = FGuid::NewGuid();
	NewNode->SkillData.SkillID = FName(*NewNode->NodeGuid.ToString());
	NewNode->SkillData.SkillName = FText::FromString(TEXT("New Skill"));
	NewNode->SkillData.UpgradeDescription = FText::FromString(TEXT("Skill Description"));
	NewNode->SkillData.SkillPointCost = 1;
	NewNode->SkillData.SkillDamage = 10.f;

	NewNode->AllocateDefaultPins();
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	NewNode->SnapToGrid(16);

	ParentGraph->AddNode(NewNode, true, bSelectNewNode);

	if (FromPin)
	{
		if (FromPin->Direction == EGPD_Output)
		{
			ParentGraph->GetSchema()->TryCreateConnection(FromPin, NewNode->GetInputPin());
		}
		else if (FromPin->Direction == EGPD_Input)
		{
			ParentGraph->GetSchema()->TryCreateConnection(NewNode->GetOutputPin(), FromPin);
		}
	}

	return NewNode;
}

UEdGraphNode* FSkillTreeSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	return USkillTreeEdGraphSchema::CreateNewSkillNode(ParentGraph, FromPin, Location, bSelectNewNode);
}
