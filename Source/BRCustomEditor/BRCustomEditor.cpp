#include "BRCustomEditor.h"
#include "Modules/ModuleManager.h"
#include "SLevelStatEditorWidget.h"
#include "SSkillTreeEditorWidget.h"
#include "LevelEditor.h" 
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SkillTreeGraphPanelNodeFactory.h"
#include "EdGraphUtilities.h"

TSharedPtr<FSkillTreeGraphPanelNodeFactory> SkillTreeGraphNodeFactory;

static const FName LevelStatEditorTabName("LevelStatEditor");
#define LOCTEXT_NAMESPACE "FBRCustomEditorModule"

// IMPLEMENT_MODULE 매크로의 첫 번째 인자를 FDefaultGameModuleImpl 대신 현재 모듈 클래스로 변경
IMPLEMENT_MODULE(FBRCustomEditorModule, BRCustomEditor);

void FBRCustomEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor StartupModule Called"));

    // 탭 스포너 등록
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelStatEditorTabName, FOnSpawnTab::CreateRaw(this, &FBRCustomEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("LevelStatEditorTab", "Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);


    FGlobalTabmanager::Get()->RegisterNomadTabSpawner("SkillTreeEditor",
        FOnSpawnTab::CreateRaw(this, &FBRCustomEditorModule::OnSpawnPluginTab_SkillTree))
        .SetDisplayName(LOCTEXT("SkillTreeEditorTab", "Skill Tree Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    SkillTreeGraphNodeFactory = MakeShareable(new FSkillTreeGraphPanelNodeFactory());
    FEdGraphUtilities::RegisterVisualNodeFactory(SkillTreeGraphNodeFactory);


    // 메뉴 등록 콜백 등록 (더 안정적인 방식)
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBRCustomEditorModule::RegisterMenus));
}

void FBRCustomEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor ShutdownModule Called"));

    // 탭 스포너 해제
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelStatEditorTabName);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("SkillTreeEditor");
    if (SkillTreeGraphNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(SkillTreeGraphNodeFactory);
        SkillTreeGraphNodeFactory.Reset();
    }


    // 메뉴 등록 콜백 해제
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FBRCustomEditorModule::RegisterMenus()
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
        FUIAction(FExecuteAction::CreateRaw(this, &FBRCustomEditorModule::HandleMenuClicked))
    );

    Section.AddMenuEntry(
        "OpenSkillTreeEditor",
        LOCTEXT("OpenSkillTreeEditor", "Skill Tree Editor"),
        LOCTEXT("OpenSkillTreeEditorTooltip", "Open the custom Skill Tree Editor window"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FBRCustomEditorModule::HandleMenuClicked_SkillTree))
    );
}

void FBRCustomEditorModule::HandleMenuClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(LevelStatEditorTabName);
}
void FBRCustomEditorModule::HandleMenuClicked_SkillTree()
{
    FGlobalTabmanager::Get()->TryInvokeTab(FName("SkillTreeEditor"));
}
TSharedRef<SDockTab> FBRCustomEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab) // NomadTab 역할을 명시
        [
            SNew(SLevelStatEditorWidget)
        ];
}
TSharedRef<SDockTab> FBRCustomEditorModule::OnSpawnPluginTab_SkillTree(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SSkillTreeEditorWidget)
        ];
}
#undef LOCTEXT_NAMESPACE