#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"

#include "../../BossRaidGame/GameData/LevelStatData.h" // FLevelStatRow 정의 포함

class SLevelStatEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelStatEditorWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** 내부 데이터 */
	TArray<TSharedPtr<FLevelStatRow>> StatRows;

	/** 리스트 뷰 위젯 */
	TSharedPtr<SListView<TSharedPtr<FLevelStatRow>>> StatListView;

	/** 행 UI 생성 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FLevelStatRow> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** 행 추가 버튼 */
	FReply OnClick_AddRow();

	/** 저장 버튼 */
	FReply OnClick_Save();
};