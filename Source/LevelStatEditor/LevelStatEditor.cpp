#include "LevelStatEditor.h"
#include "Modules/ModuleManager.h"
#include "SLevelStatEditorWidget.h"
#include "LevelEditor.h" 
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

static const FName LevelStatEditorTabName("LevelStatEditor");
#define LOCTEXT_NAMESPACE "FLevelStatEditorModule"

// IMPLEMENT_MODULE 매크로의 첫 번째 인자를 FDefaultGameModuleImpl 대신 현재 모듈 클래스로 변경
IMPLEMENT_MODULE(FLevelStatEditorModule, LevelStatEditor);

void FLevelStatEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor StartupModule Called"));

    // 탭 스포너 등록
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelStatEditorTabName, FOnSpawnTab::CreateRaw(this, &FLevelStatEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("LevelStatEditorTab", "Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    // 메뉴 등록 콜백 등록 (더 안정적인 방식)
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLevelStatEditorModule::RegisterMenus));
}

void FLevelStatEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor ShutdownModule Called"));

    // 탭 스포너 해제
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelStatEditorTabName);

    // 메뉴 등록 콜백 해제
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FLevelStatEditorModule::RegisterMenus()
{
    // 메뉴 등록 로직은 그대로 유지
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
        .TabRole(ETabRole::NomadTab) // NomadTab 역할을 명시
        [
            SNew(SLevelStatEditorWidget)
        ];
}

#undef LOCTEXT_NAMESPACE