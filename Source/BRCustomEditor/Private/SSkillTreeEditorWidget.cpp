// Fill out your copyright notice in the Description page of Project Settings.


#include "SSkillTreeEditorWidget.h"
#include "Widgets/Layout/SSplitter.h" // SSplitter
#include "IDetailsView.h"               // IDetailsView
#include "PropertyEditorModule.h"       // FPropertyEditorModule
#include "Modules/ModuleManager.h"      // FModuleManager
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraph.h"
#include "SkillTreeEdGraphNode.h" // ��� Ŭ���� ���
#include "SkillTreeEdGraphSchema.h"
#include "GraphEditor.h"
#include "Engine/DataTable.h" // ������ ���̺� ����� ���� �߰�
#include "Framework/Commands/GenericCommands.h" // FGenericCommands �ε�
#include "Widgets/Input/SButton.h"
#include "Editor.h" // GEditor ���� �� FEditorDelegates ����� ���� �߰�

void SSkillTreeEditorWidget::Construct(const FArguments& InArgs)
{


	GraphEditorCommands = MakeShareable(new FUICommandList);

	// ����(Delete) ����� �츮�� ���� �Լ��� ����
	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Delete, // �𸮾� �������� �⺻ '����' ���
		FExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::OnDeleteNodes), // ������ �Լ�
		FCanExecuteAction::CreateSP(this, &SSkillTreeEditorWidget::CanDeleteNodes) // ���� ���� ���� �Ǵ� �Լ�
	);



	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// �׷��� ��ü�� �����ϰ� ��� ������ �����մϴ�.
	SkillGraph = NewObject<USkillTreeEdGraph>();
	SkillGraph->Schema = USkillTreeEdGraphSchema::StaticClass();

	SkillGraph->SetFlags(RF_Transactional);

	SkillGraph->AddToRoot();

	// 1. FGraphEditorEvents ����ü �ν��Ͻ��� �����մϴ�.
	SGraphEditor::FGraphEditorEvents GraphEvents;

	// 2. ����ü ���� OnSelectionChanged ��������Ʈ�� �츮 �Լ��� ���ε��մϴ�.
	GraphEvents.OnSelectionChanged.BindSP(this, &SSkillTreeEditorWidget::OnSelectedNodeChanged);

	// �׷��� ������ ������ ������ ��, .GraphEvents() �μ��� ������ ���� ����ü�� �����մϴ�.
	SAssignNew(GraphEditorWidget, SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.GraphToEdit(SkillGraph)
		.GraphEvents(GraphEvents);

	// 3. ������ ������ �Ŀ�, �����͸� �о� ��带 �����ϰ� �����մϴ�.
	CreateNodesFromDataTable();
	ConnectNodes(); // �� �Լ� ������ NotifyGraphChanged()�� ���� �����ϰ� ȣ��˴ϴ�.

	// 4. ���������� ������ ���Կ� �߰��մϴ�.
	ChildSlot
		[
			SNew(SVerticalBox)

				// 1. ��� ���� ����
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SButton)
								.Text(FText::FromString(TEXT("Save")))
								.OnClicked(this, &SSkillTreeEditorWidget::OnClick_Save)
						]
				]

				// 2. ������ Splitter ����
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

FReply SSkillTreeEditorWidget::OnClick_Save()
{
	// 1. ������ ������ ���̺� �ּ��� �ε��մϴ�.
	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/GameData/DT_SkillTree.DT_SkillTree"));
	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Save Failed: DT_SkillTree�� �ε��� �� �����ϴ�."));
		return FReply::Handled();
	}

	// 2. ������ ���̺��� ���� ������ ��� �����մϴ�.
	SkillDataTable->EmptyTable();

	// 3. �׷����� ��� ��带 ��ȸ�ϸ� ���� �����͸� ������ ���̺� �ٽ� ���ϴ�.
	for (UObject* NodeObject : SkillGraph->Nodes)
	{
		if (USkillTreeEdGraphNode* SkillNode = Cast<USkillTreeEdGraphNode>(NodeObject))
		{
			// ����� ���� ��ġ(UI Position)�� SkillData�� ������Ʈ�մϴ�.
			SkillNode->SkillData.UIPosition.X = SkillNode->NodePosX;
			SkillNode->SkillData.UIPosition.Y = SkillNode->NodePosY;

			// SkillID�� RowName���� ����Ͽ� ������ ���̺� ���� �߰��մϴ�.
			SkillDataTable->AddRow(SkillNode->SkillData.SkillID, SkillNode->SkillData);
		}
	}

	// 4. ������ ���̺� �ּ��� �����Ǿ����� �����Ϳ� �˸��ϴ�. (�ּ� �̸� ���� * ǥ��)
	SkillDataTable->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Skill Tree data saved to DT_SkillTree."));

	return FReply::Handled();
}


void SSkillTreeEditorWidget::OnSelectedNodeChanged(const TSet<UObject*>& NewSelection)
{
	// ���õ� ������ �迭�� ����ϴ�.
	TArray<UObject*> SelectedNodes;
	for (UObject* Selection : NewSelection)
	{
		SelectedNodes.Add(Selection);
	}

	// ������ �信 ���õ� ��ü(���)���� ������ ǥ���ϵ��� �����մϴ�.
	DetailsView->SetObjects(SelectedNodes);
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
			NewNode->CreateNewGuid();

			// ��忡 ��ų �����͸� �����ϰ�, UI ��ġ�� �����մϴ�.
			NewNode->SkillData = *RowData;
			NewNode->NodePosX = RowData->UIPosition.X;
			NewNode->NodePosY = RowData->UIPosition.Y;
			// �׷����� ��带 �߰��ϰ�, ���� �����մϴ�.
			SkillGraph->AddNode(NewNode);
			/*NewNode->CreateInputPin();
			NewNode->CreateOutputPin();*/
			NewNode->AllocateDefaultPins();
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

TSet<UEdGraphNode*> SSkillTreeEditorWidget::GetSelectedNodes() const
{
	TSet<UEdGraphNode*> SelectedNodes;
	if (GraphEditorWidget.IsValid())
	{
		// �׷��� ������ �������� ���� ���õ� ������ �����ɴϴ�.
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
	// ���õ� ��尡 1�� �̻��� ���� true�� ��ȯ�մϴ�.
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