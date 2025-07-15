// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h" // [�߰�] ��� ���� ���
#include "EdGraph/EdGraph.h" // BreakPinLinks�� ���� �߰�
#include "Framework/Commands/GenericCommands.h" // FGraphContextMenuBuilder�� ���� �ʿ��� �� ����
#include "SkillTreeEdGraphNode.h" // USkillTreeEdGraphNode�� ����ϱ� ���� �߰�

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{

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

	//const bool bMadeConnection = Super::TryCreateConnection(A, B);

	//	
	//if (bMadeConnection)
	//{
	//	// [����] ���ῡ �����ߴٸ�, �ҽ� ����� �б� ���¸� ������Ʈ�մϴ�.
	//	// ��� ������ ������Ʈ ������ UpdateNodeBranchingState �Լ��� �߾�ȭ�Ǿ����ϴ�.
	//	UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
	//	if (USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode()))
	//	{
	//		UpdateNodeBranchingState(SourceNode);
	//	}
	//}
	//return bMadeConnection;
	const bool bMadeConnection = Super::TryCreateConnection(A, B);

	if (bMadeConnection)
	{
		// ����� �� �ɿ��� SourceNode�� TargetNode�� ��� �����ɴϴ�.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;
		USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
		USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

		if (SourceNode && TargetNode) // �� ��尡 ��� ��ȿ���� Ȯ��
		{
			// [�߰�] TargetNode�� ���� ��ų ID�� �����մϴ�.
			TargetNode->Modify();
			TargetNode->SkillData.PrerequisiteSkillID = SourceNode->SkillData.SkillID;

			// ������ �ִ� SourceNode�� �б��� ���� ������Ʈ�� �״�� �����մϴ�.
			UpdateNodeBranchingState(SourceNode);
		}
	}

	return bMadeConnection;
}

// [�߰�] CreateConnectionDrawingPolicy �Լ� ����
FConnectionDrawingPolicy* USkillTreeEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FSkillTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void USkillTreeEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText NodeCategory = FText::FromString(TEXT("Skill Tree"));
	const FText MenuDesc = FText::FromString(TEXT("Add New Skill Node"));
	const FText ToolTip = FText::FromString(TEXT("Add a new skill node to the graph."));

	TSharedPtr<FSkillTreeSchemaAction_NewNode> NewNodeAction = MakeShareable(new FSkillTreeSchemaAction_NewNode(NodeCategory, MenuDesc, ToolTip, 0));

	ContextMenuBuilder.AddAction(NewNodeAction);

	Super::GetGraphContextActions(ContextMenuBuilder);
}
void USkillTreeEdGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(FText::FromString(TEXT("Break Pin Links")));
	UE_LOG(LogTemp, Warning, TEXT("--- BreakPinLinks CALLED! This is the REAL function! ---"));

	// 1. ��ũ�� ������ �������� ����, ������ ���� �ݴ��� ������ �̸� ã�ƵӴϴ�.
	TSet<USkillTreeEdGraphNode*> NodesToUpdate;
	for (UEdGraphPin* OtherPin : TargetPin.LinkedTo)
	{
		if (USkillTreeEdGraphNode* Node = Cast<USkillTreeEdGraphNode>(OtherPin->GetOwningNode()))
		{
			NodesToUpdate.Add(Node);
		}
	}
	if (TargetPin.Direction == EGPD_Input)
	{
		if (USkillTreeEdGraphNode* NodeToClear = Cast<USkillTreeEdGraphNode>(TargetPin.GetOwningNode()))
		{
			NodeToClear->Modify();
			NodeToClear->SkillData.PrerequisiteSkillID = NAME_None;
		}
	}
	// 2. �θ��� �Լ��� ȣ���Ͽ� ���� ��ũ ���� �۾��� �����մϴ�.
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	// 3. ��ũ�� ��� ������ ��, �̸� ã�Ƶξ��� �ݴ��� ������ ���¸� ������Ʈ�մϴ�.
	for (USkillTreeEdGraphNode* NodeToUpdate : NodesToUpdate)
	{
		UpdateNodeBranchingState(NodeToUpdate);
	}

	// 4. TargetPin�� ������ ����� ���µ� ������Ʈ�մϴ�.
	if (USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(TargetPin.GetOwningNode()))
	{
		UpdateNodeBranchingState(TargetNode);
	}

}
FLinearColor USkillTreeEdGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}
//void USkillTreeEdGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
//{
//	UE_LOG(LogTemp, Warning, TEXT("BreakSInglePinlink"));
//
//	const FScopedTransaction Transaction(FText::FromString(TEXT("Break Skill Link")));
//	
//	Super::BreakSinglePinLink(SourcePin, TargetPin);
//
//	USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(SourcePin->GetOwningNode());
//	USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(TargetPin->GetOwningNode());
//
//	if (SourceNode && TargetNode)
//	{
//
//		SourceNode->Modify(); // [�߰�] SourceNode�� ���¸� ������ ���̶�� ������ �˸��ϴ�.
//
//		// �ҽ� ��尡 �б����� �ƴ϶�� NextLevelSkillID�� ���ϴ�.
//		if (!SourceNode->SkillData.bIsBranchingPoint)
//		{
//			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
//			{
//				SourceNode->SkillData.NextLevelSkillID = NAME_None;
//			}
//		}
//		// �ҽ� ��尡 �б����̶�� BranchChoices �迭���� �ش� ID�� �����մϴ�.
//		else
//		{
//			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
//		}
//
//		// �׷����� ����Ǿ����� �����Ϳ� �˸��ϴ�.
//		SourceNode->GetGraph()->NotifyGraphChanged();
//	}
//}

void USkillTreeEdGraphSchema::UpdateNodeBranchingState(USkillTreeEdGraphNode* Node) const
{
	if (!Node)
	{
		return;
	}

	UEdGraphPin* OutputPin = Node->GetOutputPin();
	if (!OutputPin)
	{
		return;
	}

	// ���� ������ Undo/Redo�� ����ϱ� ���� Modify()�� ȣ���մϴ�.
	Node->Modify();

	// ���� ����� ��� �ɿ� ����� ��ũ�� ������ �����ɴϴ�.
	const int32 ConnectionCount = OutputPin->LinkedTo.Num();

	// ������ ����ġ�� ���� ����, �ϴ� ���� ���� ������ ��� �ʱ�ȭ�մϴ�.
	Node->SkillData.NextLevelSkillID = NAME_None;
	Node->SkillData.BranchChoices.Empty();

	// ���� ������ ���� �б� ���¿� �����͸� ���� �����մϴ�.
	if (ConnectionCount >= 2)
	{
		// [�б���] ������ 2�� �̻��� ���
		Node->SkillData.bIsBranchingPoint = true;

		// ����� ��� ����� ID�� BranchChoices �迭�� �߰��մϴ�.
		for (UEdGraphPin* LinkedPin : OutputPin->LinkedTo)
		{
			if (USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(LinkedPin->GetOwningNode()))
			{
				Node->SkillData.BranchChoices.AddUnique(TargetNode->SkillData.SkillID);
			}
		}
	}
	else if (ConnectionCount == 1)
	{
		// [���� ����] ������ 1���� ���
		Node->SkillData.bIsBranchingPoint = false;

		// ����� ����� ID�� NextLevelSkillID�� �����մϴ�.
		if (UEdGraphPin* LinkedPin = OutputPin->LinkedTo[0])
		{
			if (USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(LinkedPin->GetOwningNode()))
			{
				Node->SkillData.NextLevelSkillID = TargetNode->SkillData.SkillID;
			}
		}
	}
	else // ConnectionCount == 0
	{
		// [���� ����] ������ �ϳ��� ���� ���
		Node->SkillData.bIsBranchingPoint = false;
		// (�����ʹ� �̹� ������ �ʱ�ȭ�Ǿ����Ƿ� �߰� �۾��� �ʿ� �����ϴ�.)
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
	//NewNode->SetFlags(RF_Transactional);

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
