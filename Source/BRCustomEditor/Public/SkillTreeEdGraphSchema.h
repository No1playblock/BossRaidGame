// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "SkillTreeEdGraphNode.h"   // UEdGraphNode 대신 우리 노드 클래스
#include "EdGraph/EdGraph.h"        // UEdGraph
#include "ScopedTransaction.h"      // FScopedTransaction
#include "SkillTreeEdGraphSchema.generated.h"


USTRUCT()
struct FSkillTreeSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	// FEdGraphSchemaAction의 생성자를 그대로 사용
	FSkillTreeSchemaAction_NewNode() : FEdGraphSchemaAction() {}
	FSkillTreeSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {
	}
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

class FConnectionDrawingPolicy; // 전방 선언
class UEdGraph;
class USkillTreeEdGraphNode; // 전방 선언

UCLASS()
class BRCUSTOMEDITOR_API USkillTreeEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	// [추가] 연결선을 그리는 방식을 정의하는 함수
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;

	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	
	static USkillTreeEdGraphNode* CreateNewSkillNode(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode);

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;



private:

};
