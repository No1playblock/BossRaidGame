// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class FUICommandList;
class USkillTreeEdGraph;

class SGraphEditor;
class USkillTreeEdGraph; 
class USkillTreeEdGraphNode;
class IDetailsView;
class BRCUSTOMEDITOR_API SSkillTreeEditorWidget : public SCompoundWidget, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeEditorWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
    virtual ~SSkillTreeEditorWidget();

    virtual bool MatchesContext(const FTransactionContext& InContext, const TArray<TPair<UObject*, FTransactionObjectEvent>>& TransactionObjectContexts) const override;

private:
    // 데이터 테이블로부터 노드를 생성하는 함수
    void CreateNodesFromDataTable();

    void ConnectNodes();

    void OnUndoRedo();

    /** 그래프에서 노드 선택이 변경되었을 때 호출될 콜백 함수 */
    void OnSelectedNodeChanged(const TSet<UObject*>& NewSelection);

    FReply OnClick_Save();

    void OnDeleteNodes();

    bool CanDeleteNodes() const;

    TSet<UEdGraphNode*> GetSelectedNodes() const;
private:
    // 그래프를 표시할 위젯
    TSharedPtr<SGraphEditor> GraphEditorWidget;

    FReply OnClick_Refresh();

    TSharedPtr<IDetailsView> DetailsView;

    // 현재 에디터가 편집하고 있는 그래프 객체
    UPROPERTY() // UObject이므로 UPROPERTY로 GC 방지
        TObjectPtr<USkillTreeEdGraph> SkillGraph;

    // 스킬 ID로 노드를 쉽게 찾기 위한 맵
    TMap<FName, USkillTreeEdGraphNode*> NodeMap;

    TSharedPtr<FUICommandList> GraphEditorCommands;
};
