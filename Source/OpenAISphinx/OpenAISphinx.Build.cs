// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenAISphinx : ModuleRules
{
	public OpenAISphinx(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Http", "Json", "JsonUtilities" });
	}
}
