#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "GameData/LevelStatData.h"

// 전방 선언
class UDataTable;

class SLevelStatEditorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelStatEditorWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<UDataTable> TargetDataTable;

	TArray<TSharedPtr<FLevelStatRow>> StatRows;

	TSharedPtr<SListView<TSharedPtr<FLevelStatRow>>> StatListView;

	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FLevelStatRow> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	FString SelectedDataTablePath;
private:
	FORCEINLINE FString GetSelectedDataTablePath() const { return SelectedDataTablePath; };

	FReply OnClick_AddRow();

	FReply OnClick_Save();

	void OnDataTableChanged(const FAssetData& AssetData);

	FReply OnClick_DeleteRow(TSharedPtr<FLevelStatRow> ItemToDelete); // <<< [추가]

	bool ShouldFilterDataTable(const FAssetData& AssetData) const;
};