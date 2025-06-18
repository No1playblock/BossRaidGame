// Fill out your copyright notice in the Description page of Project Settings.


#include "SSkillTreeEditorWidget.h"
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphNode.h" // ��� Ŭ���� ���
#include "SkillTreeEdGraphSchema.h"
#include "GraphEditor.h"
#include "Engine/DataTable.h" // ������ ���̺� ����� ���� �߰�
#include "Framework/Commands/GenericCommands.h" // FGenericCommands �ε�

void SSkillTreeEditorWidget::Construct(const FArguments& InArgs)
{
    // �׷��� ��ü�� �����ϰ� ��� ������ �����մϴ�.
    SkillGraph = NewObject<USkillTreeEdGraph>();
    SkillGraph->AddToRoot();

    // ������ ���̺�κ��� ������ �����մϴ�.
    CreateNodesFromDataTable();

   ConnectNodes();

    // SGraphEditor�� �����մϴ�.
    SAssignNew(GraphEditorWidget, SGraphEditor)
        .GraphToEdit(SkillGraph);

    ChildSlot
        [
            GraphEditorWidget.ToSharedRef()
        ];
}
void SSkillTreeEditorWidget::CreateNodesFromDataTable()
{
    // 1. ������ ���̺� �ּ��� �ε��մϴ�.
    // ��θ� ���� �ϵ��ڵ��ϰų�, .ini ���Ͽ��� �о�� �� �ֽ��ϴ�. ���⼭�� ���� �ε��մϴ�.
    UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/GameData/DT_SkillTree.DT_SkillTree"));
    if (!SkillDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Don't Finde DT_SkillTree"));
        return;
    }

    // 2. ��� ���� �̸��� �����ɴϴ�.
    const TArray<FName> RowNames = SkillDataTable->GetRowNames();
    NodeMap.Empty(); // �� �ʱ�ȭ

    // 3. �� �࿡ ���� ��带 �����մϴ�.
    for (const FName& RowName : RowNames)
    {
        FSkillTreeDataRow* RowData = SkillDataTable->FindRow<FSkillTreeDataRow>(RowName, TEXT(""));
        if (RowData)
        {
            // �� ��ų ��� ��ü�� �����մϴ�. �׷����� ��带 �����մϴ�.
            USkillTreeEdGraphNode* NewNode = NewObject<USkillTreeEdGraphNode>(SkillGraph);

            // ��忡 ��ų �����͸� �����ϰ�, UI ��ġ�� �����մϴ�.
            NewNode->SkillData = *RowData;
            NewNode->NodePosX = RowData->UIPosition.X;
            NewNode->NodePosY = RowData->UIPosition.Y;

            // �׷����� ��带 �߰��ϰ�, ���� �����մϴ�.
            SkillGraph->AddNode(NewNode);
            NewNode->CreateInputPin();
            NewNode->CreateOutputPin();

            // ���߿� ���� �۾��� ���� �ʿ� ��带 �����صӴϴ�.
            NodeMap.Add(RowData->SkillID, NewNode);
        }
    }
}
void SSkillTreeEditorWidget::ConnectNodes()
{
    // ��� ��带 ��ȸ�մϴ�.
    for (const auto& NodePair : NodeMap)
    {
        USkillTreeEdGraphNode* SourceNode = NodePair.Value;
        if (!SourceNode) continue;

        // �б����� �ƴ� ���: NextLevelSkillID�� ����
        if (!SourceNode->SkillData.bIsBranchingPoint && !SourceNode->SkillData.NextLevelSkillID.IsNone())
        {
            if (USkillTreeEdGraphNode* TargetNode = NodeMap.FindRef(SourceNode->SkillData.NextLevelSkillID))
            {
                // [����] ���� ��ȿ���� Ȯ���ϴ� ���� �߰�
                UEdGraphPin* SourcePin = SourceNode->GetOutputPin();
                UEdGraphPin* TargetPin = TargetNode->GetInputPin();

                if (SourcePin && TargetPin)
                {
                    SourcePin->MakeLinkTo(TargetPin);
                }
            }
        }
        // �б����� ���: BranchChoices �迭�� ��� ���� ����
        else if (SourceNode->SkillData.bIsBranchingPoint)
        {
            for (const FName& ChoiceID : SourceNode->SkillData.BranchChoices)
            {
                if (USkillTreeEdGraphNode* TargetNode = NodeMap.FindRef(ChoiceID))
                {
                    // [����] ���� ��ȿ���� Ȯ���ϴ� ���� �߰�
                    UEdGraphPin* SourcePin = SourceNode->GetOutputPin();
                    UEdGraphPin* TargetPin = TargetNode->GetInputPin();

                    if (SourcePin && TargetPin)
                    {
                        SourcePin->MakeLinkTo(TargetPin);
                    }
                }
            }
        }
    }

    // ��� ������ ���� ��, �׷��� ������ ������ ��������� �˸��ϴ�.
    GraphEditorWidget->NotifyGraphChanged();
}