// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class JoltPhysics : ModuleRules
{
	public JoltPhysics(ReadOnlyTargetRules Target) : base(Target)
	{
		bAllowConfidentialPlatformDefines = true;

		PCHUsage = PCHUsageMode.NoPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new string[] { });

		PrivateDependencyModuleNames.AddRange(new string[] { "Core" });

		PublicDefinitions.Add("WITH_JOLT_PHYSICS");

        PublicDefinitions.Add("JPH_SHARED_LIBRARY");
        PublicDefinitions.Add("JPH_BUILD_SHARED_LIBRARY");

		if (Target.bUseGameplayDebugger)
        {
            PublicDefinitions.Add("JPH_DEBUG");
            PublicDefinitions.Add("JPH_PROFILE_ENABLED");
            PublicDefinitions.Add("JPH_EXTERNAL_PROFILE");
        }

		// PublicDefinitions.Add("JPH_DOUBLE_PRECISION");
		// PublicDefinitions.Add("JPH_CROSS_PLATFORM_DETERMINISTIC");
	}
}
