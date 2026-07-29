// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

using UnrealBuildTool;

public class JoltPhysicsRuntime : ModuleRules
{
	public JoltPhysicsRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Several .cpp files in this module do a file-scope "using namespace JPH;", which leaks
		// JPH::uint64/int64 across translation-unit boundaries when Unity-built, colliding with
		// the engine's own uint64/int64 typedefs (surfaces as "reference to 'uint64' is ambiguous").
		// JoltPhysics.Build.cs already disables unity for the same reason.
		bUseUnity = false;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"JoltPhysics",
			"JoltPhysicsRuntimeCore",
		});
	}
}
