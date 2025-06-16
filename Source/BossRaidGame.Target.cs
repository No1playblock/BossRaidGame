// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BossRaidGameTarget : TargetRules
{
	public BossRaidGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		//ExtraModuleNames.Add("BossRaidGame");
        ExtraModuleNames.AddRange(new string[] { "BossRaidGame", "LevelStatEditor" });


    }
}
