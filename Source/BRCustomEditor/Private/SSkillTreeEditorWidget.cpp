// Fill out your copyright notice in the Description page of Project Settings.


#include "SSkillTreeEditorWidget.h"
#include "Widgets/Layout/SSplitter.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphNode.h"
#include "SkillTreeEdGraphSchema.h"
#include "GraphEditor.h"
#include "Engine/DataTable.h"
#include "Framework/Commands/GenericCommands.h"
#include "Widgets/Input/SButton.h"
#include "Editor.h"

void SSkillTreeEditorWidget::Construct(const FArguments& InArgs)
{
	
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Delete, // �𸮾� �������� �⺻ '����' ���
		FExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::OnDeleteNodes), // ������ �Լ�
		FCanExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::CanDeleteNodes) // ���� ���� ���� �Ǵ� �Լ�
	);

	FEditorDelegates::PostUndoRedo.AddRaw(this, &SSkillTreeEditorWidget::OnUndoRedo);



	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// �׷��� ��ü�� �����ϰ� ��� ������ ����
	SkillGraph = NewObject<USkillTreeEdGraph>();
	SkillGraph->Schema = USkillTreeEdGraphSchema::StaticClass();
	SkillGraph->SetFlags(RF_Transactional);				//�־�ߵ�
	SkillGraph->AddToRoot();

	//FGraphEditorEvents ����ü �ν��Ͻ��� ����
	SGraphEditor::FGraphEditorEvents GraphEvents;

	GraphEvents.OnSelectionChanged.BindSP(this, &SSkillTreeEditorWidget::OnSelectedNodeChanged);

	// �׷��� ������ ������ ������ ��, .GraphEvents() �μ��� ������ ���� ����ü�� ����
	SAssignNew(GraphEditorWidget, SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.GraphToEdit(SkillGraph)
		.GraphEvents(GraphEvents);

	//������ ������ �Ŀ�, �����͸� �о� ��带 �����ϰ� ����
	CreateNodesFromDataTable();
	ConnectNodes(); // �� �Լ� ������ NotifyGraphChanged()�� ���� �����ϰ� ȣ��

	//���������� ������ ���Կ� �߰�
	ChildSlot
		[
			SNew(SVerticalBox)

				//��� ���� ����
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

				//������ Splitter ����
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SSplitter)
						.Orientation(Orient_Horizontal) // �¿�� ����

						// ���� ���� (�׷��� ������)
						+ SSplitter::Slot()
						.Value(0.75f) // �ʱ� �ʺ� ����
						[
							GraphEditorWidget.ToSharedRef()
						]

						// ������ ���� (������ ��)
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
		GraphEditorWidget->NotifyGraphChanged(); // Slate ��� �ٽ� �׸���
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
		GraphEditorWidget->NotifyGraphChanged(); // Undo�� ��嵵 Slate�� �ݿ���
	}
}
FReply SSkillTreeEditorWidget::OnClick_Save()
{
	//������ ������ ���̺� �ּ��� �ε�
	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/GameData/DT_SkillTree.DT_SkillTree"));
	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Save Failed: DT_SkillTree�� �ε��� �� �����ϴ�."));
		return FReply::Handled();
	}

	//������ ���̺��� ���� ������ ��� ����
	SkillDataTable->EmptyTable();

	//�׷����� ��� ��带 ��ȸ�ϸ� ���� �����͸� ������ ���̺� �ٽ� ��.
	for (UObject* NodeObject : SkillGraph->Nodes)
	{
		if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(NodeObject))
		{
			// ����� ���� ��ġ(UI Position)�� SkillData�� ������Ʈ
			SkillNode->SkillData.UIPosition.X = SkillNode->NodePosX;
			SkillNode->SkillData.UIPosition.Y = SkillNode->NodePosY;
			SkillNode->SetFlags(RF_Transactional);
			// SkillID�� RowName���� ����Ͽ� ������ ���̺� ���� �߰�
			SkillDataTable->AddRow(SkillNode->SkillData.SkillID, SkillNode->SkillData);
		}
	}

	//������ ���̺� �ּ��� �����Ǿ����� �����Ϳ� �˸�
	SkillDataTable->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Skill Tree data saved to DT_SkillTree."));

	return FReply::Handled();
}


void SSkillTreeEditorWidget::OnSelectedNodeChanged(const TSet<UObject*>& NewSelection)
{
	// ���õ� ������ �迭�� ����
	TArray<UObject*> SelectedNodes;
	for (UObject* Selection : NewSelection)
	{
		SelectedNodes.Add(Selection);
	}

	// ������ �信 ���õ� ��ü(���)���� ������ ǥ���ϵ��� ����
	DetailsView->SetObjects(SelectedNodes);
}


void SSkillTreeEditorWidget::CreateNodesFromDataTable()
{
	// ������ ���̺� �ּ��� �ε�
	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/GameData/DT_SkillTree.DT_SkillTree"));
	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Don't Finde DT_SkillTree"));
		return;
	}

	//��� ���� �̸��� ������.
	const TArray<FName> RowNames = SkillDataTable->GetRowNames();
	NodeMap.Empty(); // �� �ʱ�ȭ

	//�� �࿡ ���� ��带 ����
	for (const FName& RowName : RowNames)
	{
		FSkillTreeDataRow* RowData = SkillDataTable->FindRow<FSkillTreeDataRow>(RowName, TEXT(""));
		if (RowData)
		{
			// �� ��ų ��� ��ü�� ����, �׷����� ��带 ����
			USkillTreeEdGraphNode* NewNode = NewObject<USkillTreeEdGraphNode>(SkillGraph);
			NewNode->CreateNewGuid();

			// ��忡 ��ų �����͸� �����ϰ�, UI ��ġ�� ����
			NewNode->SkillData = *RowData;
			NewNode->NodePosX = RowData->UIPosition.X;
			NewNode->NodePosY = RowData->UIPosition.Y;
			// �׷����� ��带 �߰��ϰ�, ���� ����
			SkillGraph->AddNode(NewNode);
			NewNode->SetFlags(RF_Transactional);		//Text Undo�� ���� �÷���
			NewNode->AllocateDefaultPins();
			
			// ���߿� ���� �۾��� ���� �ʿ� ��带 ����
			NodeMap.Add(RowData->SkillID, NewNode);
		}
	}
}

void SSkillTreeEditorWidget::ConnectNodes()
{
	// ��� ��带 ��ȸ
	for (const auto& NodePair : NodeMap)
	{
		USkillTreeEdGraphNode* SourceNode = NodePair.Value;
		if (!SourceNode) continue;

		// �б����� �ƴ� ���: NextLevelSkillID�� ����
		if (!SourceNode->SkillData.bIsBranchingPoint && !SourceNode->SkillData.NextLevelSkillID.IsNone())
		{
			if (USkillTreeEdGraphNode* TargetNode = NodeMap.FindRef(SourceNode->SkillData.NextLevelSkillID))
			{
				//���� ��ȿ���� Ȯ���ϴ� ����
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
					//���� ��ȿ���� Ȯ���ϴ� ���� �߰�
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


	GraphEditorWidget->NotifyGraphChanged();
}

TSet<UEdGraphNode*> SSkillTreeEditorWidget::GetSelectedNodes() const
{
	TSet<UEdGraphNode*> SelectedNodes;
	if (GraphEditorWidget.IsValid())
	{
		// �׷��� ������ �������� ���� ���õ� ������ ������.
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
			NodeToDelete->Modify();                // Undo ���
			NodeToDelete->DestroyNode();           // ���������� BreakAllLinks + RemoveNode ó����
		}
	}

	// Slate�� ���� ���� �˸�
	GraphEditorWidget->NotifyGraphChanged();
}
