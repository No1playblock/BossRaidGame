// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h"
#include "EdGraph/EdGraph.h"
#include "Framework/Commands/GenericCommands.h"
#include "SkillTreeEdGraphNode.h"

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{

	//같은 노드에 연결하는 것을 방지
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SamePin"));
	}

	//같은 방향의 핀끼리 연결하는 것을 방지
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Don't Connect SameDirectionPin"));
	}

	//핀의 카테고리가 다르면 연결을 막음
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
	//	연결에 성공했다면, 소스 노드의 분기 상태를 업데이트
	//	// 모든 데이터 업데이트 로직은 UpdateNodeBranchingState 함수로 중앙화
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
		// 연결된 두 핀에서 SourceNode와 TargetNode를 모두 가져옴.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;
		USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
		USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

		if (SourceNode && TargetNode) // 두 노드가 모두 유효한지 확인
		{
			//TargetNode의 선행 스킬 ID를 설정
			TargetNode->Modify();
			TargetNode->SkillData.PrerequisiteSkillID = SourceNode->SkillData.SkillID;

			// 기존에 있던 SourceNode의 분기점 상태 업데이트는 그대로 유지
			UpdateNodeBranchingState(SourceNode);
		}
	}

	return bMadeConnection;
}

//CreateConnectionDrawingPolicy 함수 구현
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

	//링크가 실제로 끊어지기 전에, 영향을 받을 반대편 노드들을 미리 찾아둠
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
	//부모의 함수를 호출하여 실제 링크 끊기 작업을 수행
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);

	//링크가 모두 끊어진 후, 미리 찾아두었던 반대편 노드들의 상태를 업데이트
	for (USkillTreeEdGraphNode* NodeToUpdate : NodesToUpdate)
	{
		UpdateNodeBranchingState(NodeToUpdate);
	}

	//TargetPin을 소유한 노드의 상태도 업데이트
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
//		// 소스 노드가 분기점이 아니라면 NextLevelSkillID를 비움
//		if (!SourceNode->SkillData.bIsBranchingPoint)
//		{
//			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
//			{
//				SourceNode->SkillData.NextLevelSkillID = NAME_None;
//			}
//		}
//		// 소스 노드가 분기점이라면 BranchChoices 배열에서 해당 ID를 제거
//		else
//		{
//			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
//		}
//
//		// 그래프가 변경되었음을 에디터에 알림
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

	// 현재 노드의 출력 핀에 연결된 링크의 개수를 가져옴
	const int32 ConnectionCount = OutputPin->LinkedTo.Num();

	// 데이터 불일치를 막기 위해, 일단 기존 연결 정보를 모두 초기화.
	Node->SkillData.NextLevelSkillID = NAME_None;
	Node->SkillData.BranchChoices.Empty();

	// 연결 개수에 따라 분기 상태와 데이터를 새로 설정
	if (ConnectionCount >= 2)
	{
		//연결이 2개 이상일 경우
		Node->SkillData.bIsBranchingPoint = true;

		// 연결된 모든 노드의 ID를 BranchChoices 배열에 추가
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
		//연결이 1개일 경우
		Node->SkillData.bIsBranchingPoint = false;

		// 연결된 노드의 ID를 NextLevelSkillID에 저장
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
		//연결이 하나도 없을 경우
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
