// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

using UnrealBuildTool;

public class JoltPhysicsRuntimeCore : ModuleRules
{
	public JoltPhysicsRuntimeCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
		});
	}
}
