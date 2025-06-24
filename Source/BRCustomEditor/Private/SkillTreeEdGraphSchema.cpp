// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h" // [추가] 방금 만든 헤더
#include "EdGraph/EdGraph.h" // BreakPinLinks를 위해 추가
#include "Framework/Commands/GenericCommands.h" // FGraphContextMenuBuilder를 위해 필요할 수 있음
#include "SkillTreeEdGraphNode.h" // USkillTreeEdGraphNode를 사용하기 위해 추가

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{

	// 1. 같은 노드에 연결하는 것을 방지합니다.
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SamePin"));
	}

	// 2. 같은 방향의 핀끼리 연결하는 것을 방지합니다.
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SameDirectionPin"));
	}

	// 3. 핀의 카테고리가 다르면 연결을 막습니다.
	if (A->PinType.PinCategory != B->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect AnotherType Pin"));
	}

	// 4. 모든 규칙을 통과했다면 연결을 허용합니다.
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connect"));
}

bool USkillTreeEdGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{

	const bool bMadeConnection = Super::TryCreateConnection(A, B);

	//if (bMadeConnection)
	//{
	//	// 연결에 성공했다면, 데이터 업데이트 로직을 추가합니다.
	//	UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
	//	UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;

	//	USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
	//	USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

	//	if (SourceNode && TargetNode)
	//	{
	//		// 소스 노드가 분기점이 아니라면 NextLevelSkillID를 업데이트합니다.
	//		if (!SourceNode->SkillData.bIsBranchingPoint)
	//		{
	//			SourceNode->SkillData.NextLevelSkillID = TargetNode->SkillData.SkillID;
	//		}
	//		// 소스 노드가 분기점이라면 BranchChoices 배열에 추가합니다.
	//		else
	//		{
	//			SourceNode->SkillData.BranchChoices.AddUnique(TargetNode->SkillData.SkillID);
	//		}

	//		// 그래프가 변경되었음을 에디터에 알립니다.
	//		SourceNode->GetGraph()->NotifyGraphChanged();
	//	}
	//}
	if (bMadeConnection)
	{
		// [정리] 연결에 성공했다면, 소스 노드의 분기 상태를 업데이트합니다.
		// 모든 데이터 업데이트 로직은 UpdateNodeBranchingState 함수로 중앙화되었습니다.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		if (USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode()))
		{
			UpdateNodeBranchingState(SourceNode);
		}
	}
	return bMadeConnection;

}

// [추가] CreateConnectionDrawingPolicy 함수 구현
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
	UE_LOG(LogTemp, Error, TEXT("--- BreakPinLinks CALLED! This is the REAL function! ---"));

	// 1. 링크가 실제로 끊어지기 전에, 영향을 받을 반대편 노드들을 미리 찾아둡니다.
	TSet<USkillTreeEdGraphNode*> NodesToUpdate;
	for (UEdGraphPin* OtherPin : TargetPin.LinkedTo)
	{
		if (USkillTreeEdGraphNode* Node = Cast<USkillTreeEdGraphNode>(OtherPin->GetOwningNode()))
		{
			NodesToUpdate.Add(Node);
		}
	}

	// 2. 부모의 함수를 호출하여 실제 링크 끊기 작업을 수행합니다.
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	// 3. 링크가 모두 끊어진 후, 미리 찾아두었던 반대편 노드들의 상태를 업데이트합니다.
	for (USkillTreeEdGraphNode* NodeToUpdate : NodesToUpdate)
	{
		UpdateNodeBranchingState(NodeToUpdate);
	}

	// 4. TargetPin을 소유한 노드의 상태도 업데이트합니다.
	if (USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(TargetPin.GetOwningNode()))
	{
		UpdateNodeBranchingState(TargetNode);
	}

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
//		SourceNode->Modify(); // [추가] SourceNode의 상태를 변경할 것이라고 엔진에 알립니다.
//
//		// 소스 노드가 분기점이 아니라면 NextLevelSkillID를 비웁니다.
//		if (!SourceNode->SkillData.bIsBranchingPoint)
//		{
//			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
//			{
//				SourceNode->SkillData.NextLevelSkillID = NAME_None;
//			}
//		}
//		// 소스 노드가 분기점이라면 BranchChoices 배열에서 해당 ID를 제거합니다.
//		else
//		{
//			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
//		}
//
//		// 그래프가 변경되었음을 에디터에 알립니다.
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

	// 변경 사항을 Undo/Redo에 기록하기 위해 Modify()를 호출합니다.
	Node->Modify();

	// 현재 노드의 출력 핀에 연결된 링크의 개수를 가져옵니다.
	const int32 ConnectionCount = OutputPin->LinkedTo.Num();

	// 데이터 불일치를 막기 위해, 일단 기존 연결 정보를 모두 초기화합니다.
	Node->SkillData.NextLevelSkillID = NAME_None;
	Node->SkillData.BranchChoices.Empty();

	// 연결 개수에 따라 분기 상태와 데이터를 새로 설정합니다.
	if (ConnectionCount >= 2)
	{
		// [분기점] 연결이 2개 이상일 경우
		Node->SkillData.bIsBranchingPoint = true;

		// 연결된 모든 노드의 ID를 BranchChoices 배열에 추가합니다.
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
		// [단일 연결] 연결이 1개일 경우
		Node->SkillData.bIsBranchingPoint = false;

		// 연결된 노드의 ID를 NextLevelSkillID에 저장합니다.
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
		// [연결 없음] 연결이 하나도 없을 경우
		Node->SkillData.bIsBranchingPoint = false;
		// (데이터는 이미 위에서 초기화되었으므로 추가 작업이 필요 없습니다.)
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
