// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class FUICommandList;
class USkillTreeEdGraph; // AddReferencedObjects�� ���� ���漱��

class SGraphEditor; // ���� ����
class USkillTreeEdGraph; // UEdGraph ��� �츮�� ���� Ŭ������ ����
class USkillTreeEdGraphNode; // ��� Ŭ���� ���� ����
class IDetailsView; // ���� ����
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
    // ������ ���̺�κ��� ��带 �����ϴ� �Լ�
    void CreateNodesFromDataTable();

    void ConnectNodes();

    void OnUndoRedo();

    /** �׷������� ��� ������ ����Ǿ��� �� ȣ��� �ݹ� �Լ� */
    void OnSelectedNodeChanged(const TSet<UObject*>& NewSelection);

    FReply OnClick_Save();

    void OnDeleteNodes();

    /** [�߰�] ��带 ������ �� �ִ��� Ȯ���ϴ� �Լ� (���õ� ��尡 ���� ���� true) */
    bool CanDeleteNodes() const;

    /** [�߰�] �׷������� ���õ� ������ ������ �������� ���� �Լ� */
    TSet<UEdGraphNode*> GetSelectedNodes() const;
private:
    // �׷����� ǥ���� ����
    TSharedPtr<SGraphEditor> GraphEditorWidget;

    FReply OnClick_Refresh();

    TSharedPtr<IDetailsView> DetailsView;

    // ���� �����Ͱ� �����ϰ� �ִ� �׷��� ��ü
    UPROPERTY() // UObject�̹Ƿ� UPROPERTY�� GC ����
        TObjectPtr<USkillTreeEdGraph> SkillGraph;

    // ��ų ID�� ��带 ���� ã�� ���� ��
    TMap<FName, USkillTreeEdGraphNode*> NodeMap;

    TSharedPtr<FUICommandList> GraphEditorCommands;
};
