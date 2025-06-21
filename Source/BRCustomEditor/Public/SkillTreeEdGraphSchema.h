// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "SkillTreeEdGraphNode.h"   // UEdGraphNode ��� �츮 ��� Ŭ����
#include "EdGraph/EdGraph.h"        // UEdGraph
#include "ScopedTransaction.h"      // FScopedTransaction
#include "SkillTreeEdGraphSchema.generated.h"


USTRUCT()
struct FSkillTreeSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	// FEdGraphSchemaAction�� �����ڸ� �״�� ���
	FSkillTreeSchemaAction_NewNode() : FEdGraphSchemaAction() {}
	FSkillTreeSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {
	}
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	// �� �׼��� �������� �� ������ ��带 �����ϴ� �Լ�
	//virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override
	//{
	//	const FScopedTransaction Transaction(FText::FromString(TEXT("Add New Skill Node")));
	//	ParentGraph->Modify(); // undo �����ϰ� ����

	//	USkillTreeEdGraphNode* NewNode = NewObject<USkillTreeEdGraphNode>(ParentGraph, NAME_None, RF_Transactional);
	//	NewNode->SetFlags(RF_Transactional); // undo �����ϰ� ����
	//	NewNode->NodeGuid = FGuid::NewGuid();

	//	NewNode->SkillData.SkillID = FName(*NewNode->NodeGuid.ToString());
	//	NewNode->SkillData.SkillName = FText::FromString(TEXT("New Skill"));
	//	NewNode->SkillData.UpgradeDescription = FText::FromString(TEXT("Skill Description"));

	//	NewNode->NodePosX = Location.X;
	//	NewNode->NodePosY = Location.Y;
	//	NewNode->SnapToGrid(16);

	//	NewNode->AllocateDefaultPins();

	//	ParentGraph->AddNode(NewNode, true, false);

	//	if (bSelectNewNode)
	//	{
	//		TSet<const UEdGraphNode*> Selected = { NewNode };
	//		ParentGraph->SelectNodeSet(Selected);
	//	}
	//	if (FromPin != nullptr)
	//	{
	//		UEdGraphPin* TargetPin = (FromPin->Direction == EGPD_Output)
	//			? NewNode->GetInputPin()
	//			: NewNode->GetOutputPin();

	//		if (FromPin->GetOwningNode() != NewNode && TargetPin)
	//		{
	//			FromPin->MakeLinkTo(TargetPin);
	//		}
	//	}
	//	// �׷��� ���� �˸� (Undo �� Slate ������Ʈ)
	//	ParentGraph->NotifyGraphChanged();

	//	return NewNode;
	//}
};

class FConnectionDrawingPolicy; // ���� ����
class UEdGraph;
UCLASS()
class BRCUSTOMEDITOR_API USkillTreeEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	// [�߰�] ���ἱ�� �׸��� ����� �����ϴ� �Լ�
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;

	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	
	static USkillTreeEdGraphNode* CreateNewSkillNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode);


	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

};
