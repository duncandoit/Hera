// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hera : ModuleRules
{
	public Hera(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput" 
		});

		PublicIncludePaths.AddRange(new string[] 
		{
			"Hera/include" 
		});
	}
}
