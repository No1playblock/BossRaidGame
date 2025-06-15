#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "../../BossRaidGame/GameData/LevelStatData.h" // FLevelStatRow ���� ����

class SLevelStatEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelStatEditorWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** ���� ������ */
	TArray<TSharedPtr<FLevelStatRow>> StatRows;

	/** ����Ʈ �� ���� */
	TSharedPtr<SListView<TSharedPtr<FLevelStatRow>>> StatListView;

	/** �� UI ���� */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FLevelStatRow> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** �� �߰� ��ư */
	FReply OnClick_AddRow();

	/** ���� ��ư */
	FReply OnClick_Save();
};