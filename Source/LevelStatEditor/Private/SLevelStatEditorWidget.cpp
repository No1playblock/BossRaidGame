#include "SLevelStatEditorWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "EditorStyleSet.h"

void SLevelStatEditorWidget::Construct(const FArguments& InArgs)
{
	// 더미 데이터 예시
	StatRows.Add(MakeShared<FLevelStatRow>());
	StatRows.Add(MakeShared<FLevelStatRow>());

	ChildSlot
		[
			SNew(SVerticalBox)

				+ SVerticalBox::Slot().AutoHeight().Padding(5)
				[
					SNew(SHorizontalBox)

						+ SHorizontalBox::Slot().AutoWidth().Padding(5)
						[
							SNew(SButton)
								.Text(FText::FromString("Add Row"))
								.OnClicked(this, &SLevelStatEditorWidget::OnClick_AddRow)
						]

						+ SHorizontalBox::Slot().AutoWidth().Padding(5)
						[
							SNew(SButton)
								.Text(FText::FromString("Save"))
								.OnClicked(this, &SLevelStatEditorWidget::OnClick_Save)
						]
				]

				+ SVerticalBox::Slot().FillHeight(1).Padding(5)
				[
					SAssignNew(StatListView, SListView<TSharedPtr<FLevelStatRow>>)
						.ItemHeight(24)
						.ListItemsSource(&StatRows)
						.OnGenerateRow(this, &SLevelStatEditorWidget::OnGenerateRow)
						.SelectionMode(ESelectionMode::None)
				]
		];
}

TSharedRef<ITableRow> SLevelStatEditorWidget::OnGenerateRow(TSharedPtr<FLevelStatRow> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FLevelStatRow>>, OwnerTable)
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot().AutoWidth().Padding(2)
				[
					SNew(STextBlock)
						.Text(FText::Format(FText::FromString("Lv. {0}"), FText::AsNumber(InItem->Level)))
				]

				+ SHorizontalBox::Slot().Padding(2)
				[
					SNew(STextBlock)
						.Text(FText::Format(FText::FromString("ATK: {0}"), FText::AsNumber(InItem->AttackPower)))
				]
		];
}

FReply SLevelStatEditorWidget::OnClick_AddRow()
{
	StatRows.Add(MakeShared<FLevelStatRow>());
	StatListView->RequestListRefresh();
	return FReply::Handled();
}

FReply SLevelStatEditorWidget::OnClick_Save()
{
	// 저장 로직 (예: CSV로 저장하거나 DataTable에 쓰기)
	UE_LOG(LogTemp, Warning, TEXT("Save Button Clicked!"));
	return FReply::Handled();
}