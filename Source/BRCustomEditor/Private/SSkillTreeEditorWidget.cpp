// Fill out your copyright notice in the Description page of Project Settings.


#include "SSkillTreeEditorWidget.h"
#include "Widgets/Layout/SSplitter.h" // SSplitter
#include "IDetailsView.h"               // IDetailsView
#include "PropertyEditorModule.h"       // FPropertyEditorModule
#include "Modules/ModuleManager.h"      // FModuleManager
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphNode.h" // 노드 클래스 헤더
#include "SkillTreeEdGraphSchema.h"
#include "GraphEditor.h"
#include "Engine/DataTable.h" // 데이터 테이블 사용을 위해 추가
#include "Framework/Commands/GenericCommands.h" // FGenericCommands 로드
#include "Widgets/Input/SButton.h"
#include "Editor.h" // GEditor 접근 및 FEditorDelegates 사용을 위해 추가

void SSkillTreeEditorWidget::Construct(const FArguments& InArgs)
{
	
	GraphEditorCommands = MakeShareable(new FUICommandList);

	// 삭제(Delete) 명령을 우리가 만든 함수와 연결
	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Delete, // 언리얼 에디터의 기본 '삭제' 명령
		FExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::OnDeleteNodes), // 실행할 함수
		FCanExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::CanDeleteNodes) // 실행 가능 여부 판단 함수
	);

	FEditorDelegates::PostUndoRedo.AddRaw(this, &SSkillTreeEditorWidget::OnUndoRedo);



	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// 그래프 객체를 생성하고 멤버 변수에 저장합니다.
	SkillGraph = NewObject<USkillTreeEdGraph>();
	SkillGraph->Schema = USkillTreeEdGraphSchema::StaticClass();
	SkillGraph->SetFlags(RF_Transactional);				//있어야됨
	SkillGraph->AddToRoot();

	// 1. FGraphEditorEvents 구조체 인스턴스를 생성합니다.
	SGraphEditor::FGraphEditorEvents GraphEvents;

	// 2. 구조체 안의 OnSelectionChanged 델리게이트에 우리 함수를 바인딩합니다.
	GraphEvents.OnSelectionChanged.BindSP(this, &SSkillTreeEditorWidget::OnSelectedNodeChanged);

	// 그래프 에디터 위젯을 생성할 때, .GraphEvents() 인수로 위에서 만든 구조체를 전달합니다.
	SAssignNew(GraphEditorWidget, SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.GraphToEdit(SkillGraph)
		.GraphEvents(GraphEvents);

	// 3. 위젯이 생성된 후에, 데이터를 읽어 노드를 생성하고 연결합니다.
	CreateNodesFromDataTable();
	ConnectNodes(); // 이 함수 내부의 NotifyGraphChanged()가 이제 안전하게 호출됩니다.

	// 4. 최종적으로 위젯을 슬롯에 추가합니다.
	ChildSlot
		[
			SNew(SVerticalBox)

				// 1. 상단 툴바 영역
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Refresh")))
								.OnClicked(this, &SSkillTreeEditorWidget::OnClick_Refresh)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Save")))
								.OnClicked(this, &SSkillTreeEditorWidget::OnClick_Save)
						]
						
				]

				// 2. 기존의 Splitter 영역
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SSplitter)
						.Orientation(Orient_Horizontal) // 좌우로 분할

						// 왼쪽 영역 (그래프 에디터)
						+ SSplitter::Slot()
						.Value(0.75f) // 초기 너비 비율
						[
							GraphEditorWidget.ToSharedRef()
						]

						// 오른쪽 영역 (디테일 뷰)
						+ SSplitter::Slot()
						.Value(0.25f)
						[
							DetailsView.ToSharedRef()
						]
				]
				
		];

	
}

FReply SSkillTreeEditorWidget::OnClick_Refresh()
{
	if (GraphEditorWidget.IsValid())
	{
		GraphEditorWidget->NotifyGraphChanged(); // Slate 노드 다시 그리기
	}
	return FReply::Handled();
}
SSkillTreeEditorWidget::~SSkillTreeEditorWidget()
{
	if (SkillGraph)
	{
		SkillGraph->RemoveFromRoot();
	}

	FEditorDelegates::PostUndoRedo.RemoveAll(this);

}
bool SSkillTreeEditorWidget::MatchesContext(const FTransactionContext& InContext, const TArray<TPair<UObject*, FTransactionObjectEvent>>& TransactionObjectContexts) const
{
	return true;
}
void SSkillTreeEditorWidget::OnUndoRedo()
{

	UE_LOG(LogTemp, Warning, TEXT("OnUndoRedo"));
	if (GraphEditorWidget.IsValid())
	{
		GraphEditorWidget->ClearSelectionSet();
		GraphEditorWidget->NotifyGraphChanged(); // Undo된 노드도 Slate에 반영됨
	}
}
FReply SSkillTreeEditorWidget::OnClick_Save()
{
	// 1. 저장할 데이터 테이블 애셋을 로드합니다.
	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/GameData/DT_SkillTree.DT_SkillTree"));
	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Save Failed: DT_SkillTree를 로드할 수 없습니다."));
		return FReply::Handled();
	}

	// 2. 데이터 테이블의 기존 내용을 모두 삭제합니다.
	SkillDataTable->EmptyTable();

	// 3. 그래프의 모든 노드를 순회하며 현재 데이터를 데이터 테이블에 다시 씁니다.
	for (UObject* NodeObject : SkillGraph->Nodes)
	{
		if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(NodeObject))
		{
			// 노드의 현재 위치(UI Position)를 SkillData에 업데이트합니다.
			SkillNode->SkillData.UIPosition.X = SkillNode->NodePosX;
			SkillNode->SkillData.UIPosition.Y = SkillNode->NodePosY;
			SkillNode->SetFlags(RF_Transactional);
			// SkillID를 RowName으로 사용하여 데이터 테이블에 행을 추가합니다.
			SkillDataTable->AddRow(SkillNode->SkillData.SkillID, SkillNode->SkillData);
		}
	}

	// 4. 데이터 테이블 애셋이 수정되었음을 에디터에 알립니다. (애셋 이름 옆에 * 표시)
	SkillDataTable->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Skill Tree data saved to DT_SkillTree."));

	return FReply::Handled();
}


void SSkillTreeEditorWidget::OnSelectedNodeChanged(const TSet<UObject*>& NewSelection)
{
	// 선택된 노드들의 배열을 만듭니다.
	TArray<UObject*> SelectedNodes;
	for (UObject* Selection : NewSelection)
	{
		SelectedNodes.Add(Selection);
	}

	// 디테일 뷰에 선택된 객체(노드)들의 정보를 표시하도록 설정합니다.
	DetailsView->SetObjects(SelectedNodes);
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
			NewNode->CreateNewGuid();

			// 노드에 스킬 데이터를 복사하고, UI 위치를 설정합니다.
			NewNode->SkillData = *RowData;
			NewNode->NodePosX = RowData->UIPosition.X;
			NewNode->NodePosY = RowData->UIPosition.Y;
			// 그래프에 노드를 추가하고, 핀을 생성합니다.
			SkillGraph->AddNode(NewNode);
			NewNode->SetFlags(RF_Transactional);		//Text Undo를 위한 플래그
			NewNode->AllocateDefaultPins();
			
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

TSet<UEdGraphNode*> SSkillTreeEditorWidget::GetSelectedNodes() const
{
	TSet<UEdGraphNode*> SelectedNodes;
	if (GraphEditorWidget.IsValid())
	{
		// 그래프 에디터 위젯에서 현재 선택된 노드들을 가져옵니다.
		const TSet<UObject*>& SelectedObjects = GraphEditorWidget->GetSelectedNodes();
		for (UObject* Obj : SelectedObjects)
		{
			if (UEdGraphNode* Node = Cast<UEdGraphNode>(Obj))
			{
				SelectedNodes.Add(Node);
			}
		}
	}
	return SelectedNodes;
}

bool SSkillTreeEditorWidget::CanDeleteNodes() const
{
	// 선택된 노드가 1개 이상일 때만 true를 반환합니다.
	return GetSelectedNodes().Num() > 0;
}



void SSkillTreeEditorWidget::OnDeleteNodes()
{
	if (!GraphEditorWidget.IsValid())
	{
		return;
	}

	const TSet<UEdGraphNode*> SelectedNodes = GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
	{
		return;
	}

	const FScopedTransaction Transaction(FText::FromString(TEXT("Delete Skill Nodes")));

	SkillGraph->Modify();

	for (UEdGraphNode* NodeToDelete : SelectedNodes)
	{
		if (NodeToDelete)
		{
			NodeToDelete->Modify();                // Undo 기록
			NodeToDelete->DestroyNode();           // 내부적으로 BreakAllLinks + RemoveNode 처리됨
		}
	}

	// Slate에 변경 사항 알림
	GraphEditorWidget->NotifyGraphChanged();
}
