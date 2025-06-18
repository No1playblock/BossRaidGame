using UnrealBuildTool;

public class BRCustomEditor : ModuleRules
{
    public BRCustomEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore",
                                                                "EditorStyle", "UnrealEd", "PropertyEditor", "Projects",
                                                                    "BRCustomEditor", "ToolMenus", "BossRaidGame", "GraphEditor", "BlueprintGraph"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });

        PrivateIncludePaths.AddRange(new string[] {
            "BRCustomEditor/Public"
        }); 
    }
}