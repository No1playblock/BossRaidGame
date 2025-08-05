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

    
};