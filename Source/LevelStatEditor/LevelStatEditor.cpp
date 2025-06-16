#include "LevelStatEditor.h"
#include "Modules/ModuleManager.h"
#include "SLevelStatEditorWidget.h"
#include "LevelEditor.h" 
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

static const FName LevelStatEditorTabName("LevelStatEditor");
#define LOCTEXT_NAMESPACE "FLevelStatEditorModule"

// IMPLEMENT_MODULE ��ũ���� ù ��° ���ڸ� FDefaultGameModuleImpl ��� ���� ��� Ŭ������ ����
IMPLEMENT_MODULE(FLevelStatEditorModule, LevelStatEditor);

void FLevelStatEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor StartupModule Called"));

    // �� ������ ���
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelStatEditorTabName, FOnSpawnTab::CreateRaw(this, &FLevelStatEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("LevelStatEditorTab", "Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    // �޴� ��� �ݹ� ��� (�� �������� ���)
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLevelStatEditorModule::RegisterMenus));
}

void FLevelStatEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor ShutdownModule Called"));

    // �� ������ ����
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelStatEditorTabName);

    // �޴� ��� �ݹ� ����
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FLevelStatEditorModule::RegisterMenus()
{
    // �޴� ��� ������ �״�� ����
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
    FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");

    Section.AddMenuEntry(
        "OpenLevelStatEditor",
        LOCTEXT("OpenLevelStatEditor", "Level Stat Editor"),
        LOCTEXT("OpenLevelStatEditorTooltip", "Open the custom Level Stat Editor window"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FLevelStatEditorModule::HandleMenuClicked))
    );
}

void FLevelStatEditorModule::HandleMenuClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(LevelStatEditorTabName);
}

TSharedRef<SDockTab> FLevelStatEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab) // NomadTab ������ ���
        [
            SNew(SLevelStatEditorWidget)
        ];
}

#undef LOCTEXT_NAMESPACE