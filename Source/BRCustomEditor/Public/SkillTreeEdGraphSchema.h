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
};

class FConnectionDrawingPolicy; // ���� ����
class UEdGraph;
class USkillTreeEdGraphNode; // ���� ����

UCLASS()
class BRCUSTOMEDITOR_API USkillTreeEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	// [�߰�] ���ἱ�� �׸��� ����� �����ϴ� �Լ�
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;

	//virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	

	static USkillTreeEdGraphNode* CreateNewSkillNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode);

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

private:
	void UpdateNodeBranchingState(USkillTreeEdGraphNode* Node) const;

};
