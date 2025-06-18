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

// IMPLEMENT_MODULE ��ũ���� ù ��° ���ڸ� FDefaultGameModuleImpl ��� ���� ��� Ŭ������ ����
IMPLEMENT_MODULE(FBRCustomEditorModule, BRCustomEditor);

void FBRCustomEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor StartupModule Called"));

    // �� ������ ���
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LevelStatEditorTabName, FOnSpawnTab::CreateRaw(this, &FBRCustomEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("LevelStatEditorTab", "Level Stat Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    // �޴� ��� �ݹ� ��� (�� �������� ���)
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBRCustomEditorModule::RegisterMenus));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner("SkillTreeEditor",
        FOnSpawnTab::CreateRaw(this, &FBRCustomEditorModule::OnSpawnPluginTab_SkillTree))
        .SetDisplayName(LOCTEXT("SkillTreeEditorTab", "Skill Tree Editor"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
    SkillTreeGraphNodeFactory = MakeShareable(new FSkillTreeGraphPanelNodeFactory());
    FEdGraphUtilities::RegisterVisualNodeFactory(SkillTreeGraphNodeFactory);

}

void FBRCustomEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("LevelStatEditor ShutdownModule Called"));

    // �� ������ ����
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LevelStatEditorTabName);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("SkillTreeEditor");
    if (SkillTreeGraphNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(SkillTreeGraphNodeFactory);
        SkillTreeGraphNodeFactory.Reset();
    }


    // �޴� ��� �ݹ� ����
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FBRCustomEditorModule::RegisterMenus()
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
        .TabRole(ETabRole::NomadTab) // NomadTab ������ ���
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