// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class FUICommandList;
class USkillTreeEdGraph; // AddReferencedObjects를 위해 전방선언

class SGraphEditor; // 전방 선언
class USkillTreeEdGraph; // UEdGraph 대신 우리가 만든 클래스로 변경
class USkillTreeEdGraphNode; // 노드 클래스 전방 선언
class IDetailsView; // 전방 선언
class BRCUSTOMEDITOR_API SSkillTreeEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeEditorWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
    virtual ~SSkillTreeEditorWidget();

   /* virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override;*/
private:
    // 데이터 테이블로부터 노드를 생성하는 함수
    void CreateNodesFromDataTable();

    void ConnectNodes();

    void OnUndoRedo();

    /** 그래프에서 노드 선택이 변경되었을 때 호출될 콜백 함수 */
    void OnSelectedNodeChanged(const TSet<UObject*>& NewSelection);

    FReply OnClick_Save();

    void OnDeleteNodes();

    /** [추가] 노드를 삭제할 수 있는지 확인하는 함수 (선택된 노드가 있을 때만 true) */
    bool CanDeleteNodes() const;

    /** [추가] 그래프에서 선택된 노드들의 집합을 가져오는 헬퍼 함수 */
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
