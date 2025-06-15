#include "LevelStatEditor.h"
#include "Modules/ModuleManager.h"
#include "SLevelStatEditorWidget.h"
#include "LevelEditor.h" 
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

IMPLEMENT_MODULE(FDefaultGameModuleImpl, LevelStatEditor);

#define LOCTEXT_NAMESPACE "FLevelStatEditorModule"

void FLevelStatEditorModule::RegisterMenus()
{
    UE_LOG(LogTemp, Warning, TEXT("RegisterMenus() Called"));
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

void FLevelStatEditorModule::StartupModule()
{
   /* FGlobalTabmanager::Get()->RegisterNomadTabSpawner("LevelStatEditor", FOnSpawnTab::CreateRaw(this, &FLevelStatEditorModule::OnSpawnPluginTab))
        .SetDisplayName(FText::FromString("Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);*/
    UE_LOG(LogTemp, Warning, TEXT("StartupModule Called"));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner("LevelStatEditor",
        FOnSpawnTab::CreateRaw(this, &FLevelStatEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("LevelStatEditorTab", "Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    RegisterMenus();
    // 메뉴 등록
    //UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLevelStatEditorModule::RegisterMenus));
}
void FLevelStatEditorModule::HandleMenuClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(FName("LevelStatEditor"));
}
void FLevelStatEditorModule::ShutdownModule()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("LevelStatEditor");

    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

TSharedRef<SDockTab> FLevelStatEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        [
            SNew(SLevelStatEditorWidget)
        ];
}