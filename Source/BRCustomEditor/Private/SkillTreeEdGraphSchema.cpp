// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdGraphSchema.h"
#include "SkillTreeConnectionDrawingPolicy.h" // [추가] 방금 만든 헤더
#include "EdGraph/EdGraph.h" // BreakPinLinks를 위해 추가
#include "Framework/Commands/GenericCommands.h" // FGraphContextMenuBuilder를 위해 필요할 수 있음
#include "SkillTreeEdGraphNode.h" // USkillTreeEdGraphNode를 사용하기 위해 추가

const FPinConnectionResponse USkillTreeEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UE_LOG(LogTemp, Warning, TEXT("CanCreateConnection"));
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

	// [이전 코드의 문제점]
	// A 또는 B가 nullptr인 경우를 고려하지 않아 드래그 시작이 거부되었습니다.
	// 하지만 이 함수는 드래그를 시작할 때도 B=nullptr 상태로 호출될 수 있습니다.

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

	if (bMadeConnection)
	{
		// 연결에 성공했다면, 데이터 업데이트 로직을 추가합니다.
		UEdGraphPin* OutputPin = (A->Direction == EGPD_Output) ? A : B;
		UEdGraphPin* InputPin = (A->Direction == EGPD_Input) ? A : B;

		USkillTreeEdGraphNode* SourceNode = Cast<USkillTreeEdGraphNode>(OutputPin->GetOwningNode());
		USkillTreeEdGraphNode* TargetNode = Cast<USkillTreeEdGraphNode>(InputPin->GetOwningNode());

		if (SourceNode && TargetNode)
		{
			// 소스 노드가 분기점이 아니라면 NextLevelSkillID를 업데이트합니다.
			if (!SourceNode->SkillData.bIsBranchingPoint)
			{
				SourceNode->SkillData.NextLevelSkillID = TargetNode->SkillData.SkillID;
			}
			// 소스 노드가 분기점이라면 BranchChoices 배열에 추가합니다.
			else
			{
				SourceNode->SkillData.BranchChoices.AddUnique(TargetNode->SkillData.SkillID);
			}

			// 그래프가 변경되었음을 에디터에 알립니다.
			SourceNode->GetGraph()->NotifyGraphChanged();
		}
	}

	return bMadeConnection;

}

// [추가] CreateConnectionDrawingPolicy 함수 구현
FConnectionDrawingPolicy* USkillTreeEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	
	//UE_LOG(LogTemp, Warning, TEXT("CreateConnectionDrawingPolicy"));

	return new FSkillTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void USkillTreeEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText NodeCategory = FText::FromString(TEXT("Skill Tree"));
	const FText MenuDesc = FText::FromString(TEXT("Add New Skill Node"));
	const FText ToolTip = FText::FromString(TEXT("Add a new skill node to the graph."));

	TSharedPtr<FSkillTreeSchemaAction_NewNode> NewNodeAction = MakeShareable(new FSkillTreeSchemaAction_NewNode(NodeCategory, MenuDesc, ToolTip, 0));

	// 핀에서 드래그하여 메뉴를 연 경우 (방향과 상관없이)
	if (ContextMenuBuilder.FromPin)
	{
		ContextMenuBuilder.AddAction(NewNodeAction);
	}
	// 빈 공간에서 메뉴를 연 경우
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
		SourceNode->Modify(); // [추가] SourceNode의 상태를 변경할 것이라고 엔진에 알립니다.

		// 소스 노드가 분기점이 아니라면 NextLevelSkillID를 비웁니다.
		if (!SourceNode->SkillData.bIsBranchingPoint)
		{
			if (SourceNode->SkillData.NextLevelSkillID == TargetNode->SkillData.SkillID)
			{
				SourceNode->SkillData.NextLevelSkillID = NAME_None;
			}
		}
		// 소스 노드가 분기점이라면 BranchChoices 배열에서 해당 ID를 제거합니다.
		else
		{
			SourceNode->SkillData.BranchChoices.Remove(TargetNode->SkillData.SkillID);
		}

		// 그래프가 변경되었음을 에디터에 알립니다.
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
	NewNode->SetFlags(RF_Transactional);

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
