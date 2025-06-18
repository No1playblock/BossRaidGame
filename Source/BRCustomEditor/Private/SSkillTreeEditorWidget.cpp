// Fill out your copyright notice in the Description page of Project Settings.


#include "SSkillTreeEditorWidget.h"
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphNode.h" // 노드 클래스 헤더
#include "SkillTreeEdGraphSchema.h"
#include "GraphEditor.h"
#include "Engine/DataTable.h" // 데이터 테이블 사용을 위해 추가
#include "Framework/Commands/GenericCommands.h" // FGenericCommands 로드

void SSkillTreeEditorWidget::Construct(const FArguments& InArgs)
{
    // 그래프 객체를 생성하고 멤버 변수에 저장합니다.
    SkillGraph = NewObject<USkillTreeEdGraph>();
    SkillGraph->AddToRoot();

    // 데이터 테이블로부터 노드들을 생성합니다.
    CreateNodesFromDataTable();

   ConnectNodes();

    // SGraphEditor를 생성합니다.
    SAssignNew(GraphEditorWidget, SGraphEditor)
        .GraphToEdit(SkillGraph);

    ChildSlot
        [
            GraphEditorWidget.ToSharedRef()
        ];
}
void SSkillTreeEditorWidget::CreateNodesFromDataTable()
{
    // 1. 데이터 테이블 애셋을 로드합니다.
    // 경로를 직접 하드코딩하거나, .ini 파일에서 읽어올 수 있습니다. 여기서는 직접 로드합니다.
    UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/GameData/DT_SkillTree.DT_SkillTree"));
    if (!SkillDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Don't Finde DT_SkillTree"));
        return;
    }

    // 2. 모든 행의 이름을 가져옵니다.
    const TArray<FName> RowNames = SkillDataTable->GetRowNames();
    NodeMap.Empty(); // 맵 초기화

    // 3. 각 행에 대해 노드를 생성합니다.
    for (const FName& RowName : RowNames)
    {
        FSkillTreeDataRow* RowData = SkillDataTable->FindRow<FSkillTreeDataRow>(RowName, TEXT(""));
        if (RowData)
        {
            // 새 스킬 노드 객체를 생성합니다. 그래프가 노드를 소유합니다.
            USkillTreeEdGraphNode* NewNode = NewObject<USkillTreeEdGraphNode>(SkillGraph);

            // 노드에 스킬 데이터를 복사하고, UI 위치를 설정합니다.
            NewNode->SkillData = *RowData;
            NewNode->NodePosX = RowData->UIPosition.X;
            NewNode->NodePosY = RowData->UIPosition.Y;

            // 그래프에 노드를 추가하고, 핀을 생성합니다.
            SkillGraph->AddNode(NewNode);
            NewNode->CreateInputPin();
            NewNode->CreateOutputPin();

            // 나중에 연결 작업을 위해 맵에 노드를 저장해둡니다.
            NodeMap.Add(RowData->SkillID, NewNode);
        }
    }
}
void SSkillTreeEditorWidget::ConnectNodes()
{
    // 모든 노드를 순회합니다.
    for (const auto& NodePair : NodeMap)
    {
        USkillTreeEdGraphNode* SourceNode = NodePair.Value;
        if (!SourceNode) continue;

        // 분기점이 아닌 경우: NextLevelSkillID로 연결
        if (!SourceNode->SkillData.bIsBranchingPoint && !SourceNode->SkillData.NextLevelSkillID.IsNone())
        {
            if (USkillTreeEdGraphNode* TargetNode = NodeMap.FindRef(SourceNode->SkillData.NextLevelSkillID))
            {
                // [수정] 핀이 유효한지 확인하는 로직 추가
                UEdGraphPin* SourcePin = SourceNode->GetOutputPin();
                UEdGraphPin* TargetPin = TargetNode->GetInputPin();

                if (SourcePin && TargetPin)
                {
                    SourcePin->MakeLinkTo(TargetPin);
                }
            }
        }
        // 분기점인 경우: BranchChoices 배열의 모든 노드와 연결
        else if (SourceNode->SkillData.bIsBranchingPoint)
        {
            for (const FName& ChoiceID : SourceNode->SkillData.BranchChoices)
            {
                if (USkillTreeEdGraphNode* TargetNode = NodeMap.FindRef(ChoiceID))
                {
                    // [수정] 핀이 유효한지 확인하는 로직 추가
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

    // 모든 연결이 끝난 후, 그래프 에디터 위젯에 변경사항을 알립니다.
    GraphEditorWidget->NotifyGraphChanged();
}