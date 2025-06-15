#pragma once

#include "CoreMinimal.h"

class FLevelStatEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    
    virtual void ShutdownModule() override;

    void RegisterMenus();

    void HandleMenuClicked();
private:
    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& Args);

    
};