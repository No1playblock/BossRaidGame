#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FLevelStatEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    
    virtual void ShutdownModule() override;

   
private:

    void RegisterMenus();

    void HandleMenuClicked();
    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& Args);

    
};