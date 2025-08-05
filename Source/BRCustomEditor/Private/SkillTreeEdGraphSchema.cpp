// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h"
#include "EdGraph/EdGraph.h"
#include "Framework/Commands/GenericCommands.h"
#include "SkillTreeEdGraphNode.h"

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{

	//���� ��忡 �����ϴ� ���� ����
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SamePin"));
	}

	//���� ������ �ɳ��� �����ϴ� ���� ����
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SameDirectionPin"));
	}

	//���� ī�װ��� �ٸ��� ������ ����
	if (A->PinType.PinCategory != B->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect AnotherType Pin"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connect"));
}

bool USkillTreeEdGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{

	//const bool bMadeConnection = Super::TryCreateConnection(A, B);

	//	
	//if (bMadeConnection)
	//{
	//	���ῡ �����ߴٸ�, �ҽ� ����� �б� ���¸� ������Ʈ
	//	// ��� ������ ������Ʈ ������ UpdateNodeBranchingState �Լ��� �߾�ȭ
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
		// ����� �� �ɿ��� SourceNode�� TargetNode�� ��� ������.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;
		USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
		USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

		if (SourceNode && TargetNode) // �� ��尡 ��� ��ȿ���� Ȯ��
		{
			//TargetNode�� ���� ��ų ID�� ����
			TargetNode->Modify();
			TargetNode->SkillData.PrerequisiteSkillID = SourceNode->SkillData.SkillID;

			// ������ �ִ� SourceNode�� �б��� ���� ������Ʈ�� �״�� ����
			UpdateNodeBranchingState(SourceNode);
		}
	}

	return bMadeConnection;
}

//CreateConnectionDrawingPolicy �Լ� ����
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

	//��ũ�� ������ �������� ����, ������ ���� �ݴ��� ������ �̸� ã�Ƶ�
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
	//�θ��� �Լ��� ȣ���Ͽ� ���� ��ũ ���� �۾��� ����
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	//��ũ�� ��� ������ ��, �̸� ã�Ƶξ��� �ݴ��� ������ ���¸� ������Ʈ
	for (USkillTreeEdGraphNode* NodeToUpdate : NodesToUpdate)
	{
		UpdateNodeBranchingState(NodeToUpdate);
	}

	//TargetPin�� ������ ����� ���µ� ������Ʈ
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
//		SourceNode->Modify(); 
//
//		// �ҽ� ��尡 �б����� �ƴ϶�� NextLevelSkillID�� ���
//		if (!SourceNode->SkillData.bIsBranchingPoint)
//		{
//			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
//			{
//				SourceNode->SkillData.NextLevelSkillID = NAME_None;
//			}
//		}
//		// �ҽ� ��尡 �б����̶�� BranchChoices �迭���� �ش� ID�� ����
//		else
//		{
//			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
//		}
//
//		// �׷����� ����Ǿ����� �����Ϳ� �˸�
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

	Node->Modify();

	// ���� ����� ��� �ɿ� ����� ��ũ�� ������ ������
	const int32 ConnectionCount = OutputPin->LinkedTo.Num();

	// ������ ����ġ�� ���� ����, �ϴ� ���� ���� ������ ��� �ʱ�ȭ.
	Node->SkillData.NextLevelSkillID = NAME_None;
	Node->SkillData.BranchChoices.Empty();

	// ���� ������ ���� �б� ���¿� �����͸� ���� ����
	if (ConnectionCount >= 2)
	{
		//������ 2�� �̻��� ���
		Node->SkillData.bIsBranchingPoint = true;

		// ����� ��� ����� ID�� BranchChoices �迭�� �߰�
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
		//������ 1���� ���
		Node->SkillData.bIsBranchingPoint = false;

		// ����� ����� ID�� NextLevelSkillID�� ����
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
		//������ �ϳ��� ���� ���
		Node->SkillData.bIsBranchingPoint = false;

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
