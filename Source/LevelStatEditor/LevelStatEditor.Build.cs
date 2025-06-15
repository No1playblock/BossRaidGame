using UnrealBuildTool;

public class LevelStatEditor : ModuleRules
{
    public LevelStatEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore",
                                                                "EditorStyle", "UnrealEd", "PropertyEditor", "Projects", 
                                                                    "LevelEditor", "ToolMenus", "BossRaidGame" 
        });

        PrivateIncludePaths.AddRange(new string[] {
            "LevelStatEditor/Public"
        }); 
    }
}