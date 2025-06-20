#include "SLevelStatEditorWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Views/STableRow.h"
#include "PropertyCustomizationHelpers.h"
#include "Editor.h"

void SLevelStatEditorWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(5)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.Padding(5)
						[
							SNew(SObjectPropertyEntryBox)
								.AllowedClass(UDataTable::StaticClass())
								.ObjectPath(this, &SLevelStatEditorWidget::GetSelectedDataTablePath)
								.OnObjectChanged(this, &SLevelStatEditorWidget::OnDataTableChanged)
								.DisplayBrowse(true)
								.DisplayUseSelected(true)
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5)
						[
							SNew(SButton)
								.Text(FText::FromString("Add Row"))
								.OnClicked(this, &SLevelStatEditorWidget::OnClick_AddRow)
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5)
						[
							SNew(SButton)
								.Text(FText::FromString("Save"))
								.OnClicked(this, &SLevelStatEditorWidget::OnClick_Save)
						]
				]

				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(5)
				[
					SAssignNew(StatListView, SListView<TSharedPtr<FLevelStatRow>>)
						.ItemHeight(24)
						.ListItemsSource(&StatRows)
						.OnGenerateRow(this, &SLevelStatEditorWidget::OnGenerateRow)
						.SelectionMode(ESelectionMode::None)
						.HeaderRow
						(
							SNew(SHeaderRow)
							+ SHeaderRow::Column("Level").DefaultLabel(FText::FromString("Level")).FillWidth(0.1f)
							+ SHeaderRow::Column("AttackPower").DefaultLabel(FText::FromString("Attack Power")).FillWidth(0.17f)
							+ SHeaderRow::Column("AttackSpeed").DefaultLabel(FText::FromString("Attack Speed(Rate)")).FillWidth(0.17f)
							+ SHeaderRow::Column("SkillPower").DefaultLabel(FText::FromString("Skill Power")).FillWidth(0.17f)
							+ SHeaderRow::Column("CooldownRate").DefaultLabel(FText::FromString("Cooldown Rate")).FillWidth(0.17f)
							+ SHeaderRow::Column("MoveSpeed").DefaultLabel(FText::FromString("Move Speed(Rate)")).FillWidth(0.17f)
							+ SHeaderRow::Column("DeleteButtonColumn").DefaultLabel(FText::GetEmpty()).FillWidth(0.05f)
						)
				]
		];
}

TSharedRef<ITableRow> SLevelStatEditorWidget::OnGenerateRow(TSharedPtr<FLevelStatRow> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FLevelStatRow>>, OwnerTable)
		.Padding(FMargin(0.0f, 2.0f)) 
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot().FillWidth(0.1f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<int32>).Value_Lambda([InItem] { return InItem->Level; }).OnValueChanged_Lambda([InItem](int32 NewValue) { InItem->Level = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.17f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<float>).Value_Lambda([InItem] { return InItem->AttackPower; }).OnValueChanged_Lambda([InItem](float NewValue) { InItem->AttackPower = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.17f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<float>).Value_Lambda([InItem] { return InItem->AttackSpeed; }).OnValueChanged_Lambda([InItem](float NewValue) { InItem->AttackSpeed = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.17f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<float>).Value_Lambda([InItem] { return InItem->SkillPower; }).OnValueChanged_Lambda([InItem](float NewValue) { InItem->SkillPower = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.17f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<float>).Value_Lambda([InItem] { return InItem->SkillCooldownRate; }).OnValueChanged_Lambda([InItem](float NewValue) { InItem->SkillCooldownRate = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.17f)
						[
							SNew(SBox).Padding(3.0f, 0.0f)[SNew(SNumericEntryBox<float>).Value_Lambda([InItem] { return InItem->MoveSpeed; }).OnValueChanged_Lambda([InItem](float NewValue) { InItem->MoveSpeed = NewValue; })]
						]
						+ SHorizontalBox::Slot().FillWidth(0.05f)
						[
							SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(SButton).Text(FText::FromString(TEXT("X"))).OnClicked(this, &SLevelStatEditorWidget::OnClick_DeleteRow, InItem)]
						]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.f, 4.f, 0.f, 0.f) 
				[
					SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")) 
						.Padding(1.0f) 
				]
		];
}

void SLevelStatEditorWidget::OnDataTableChanged(const FAssetData& AssetData)
{
	StatRows.Empty();

	UDataTable* LoadedTable = Cast<UDataTable>(AssetData.GetAsset());
	if (LoadedTable)
	{
		TargetDataTable = LoadedTable;
		SelectedDataTablePath = AssetData.ObjectPath.ToString();

		for (auto it : LoadedTable->GetRowMap())
		{
			FName RowName = it.Key;
			FLevelStatRow* RowData = (FLevelStatRow*)it.Value;

			if (RowData)
			{
				TSharedPtr<FLevelStatRow> NewRow = MakeShared<FLevelStatRow>(*RowData);
				StatRows.Add(NewRow);
			}
		}
	}
	else
	{
		TargetDataTable = nullptr;
		SelectedDataTablePath.Empty();
	}

	StatListView->RequestListRefresh();
}
FReply SLevelStatEditorWidget::OnClick_AddRow()
{
	int32 NewLevel = 1;
	if (StatRows.Num() > 0)
	{
		NewLevel = StatRows.Last()->Level + 1;
	}

	TSharedPtr<FLevelStatRow> NewRow = MakeShared<FLevelStatRow>();
	NewRow->Level = NewLevel;

	StatRows.Add(NewRow);

	StatListView->RequestListRefresh();
	return FReply::Handled();
}

FReply SLevelStatEditorWidget::OnClick_DeleteRow(TSharedPtr<FLevelStatRow> ItemToDelete)
{
	if (ItemToDelete.IsValid())
	{
		StatRows.Remove(ItemToDelete);
		StatListView->RequestListRefresh();
	}

	return FReply::Handled();
}

FReply SLevelStatEditorWidget::OnClick_Save()
{
	if (!TargetDataTable.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target DataTable is not valid!"));
		return FReply::Handled();
	}

	TargetDataTable->EmptyTable();

	for (const TSharedPtr<FLevelStatRow>& RowData : StatRows)
	{
		FName RowName = FName(*FString::FromInt(RowData->Level));
		TargetDataTable->AddRow(RowName, *RowData);
	}

	TargetDataTable->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("DataTable saved successfully: %s"), *TargetDataTable->GetName());

	return FReply::Handled();
}
