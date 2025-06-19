#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FBRCustomEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    
    virtual void ShutdownModule() override;

   
private:

    void RegisterMenus();

    void HandleMenuClicked();

    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& Args);

    void HandleMenuClicked_SkillTree();

    TSharedRef<SDockTab> OnSpawnPluginTab_SkillTree(const FSpawnTabArgs& Args);

    // [추가] 스킬 트리 에디터 메뉴 클릭 핸들러
    
};