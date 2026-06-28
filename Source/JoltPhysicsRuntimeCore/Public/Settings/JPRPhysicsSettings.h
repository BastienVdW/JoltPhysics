// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UObject/SoftObjectPtr.h"

#include "JPRPhysicsSettings.generated.h"

/** Configuration shared by Unreal systems backed by a Jolt physics world. */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Jolt Physics"))
class JOLTPHYSICSRUNTIMECORE_API UJPRPhysicsSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_UCLASS_BODY()

public:
	/// Maximum number of worker threads available to the physics job system.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	int32 MaxNumThreads = 4;

	/// Number of collision sub-steps performed by each physics update.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	int32 CollisionSteps = 1;

	/// Maximum number of rigid bodies in the physics world.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	uint32 MaxBodies = 65536;

	/// Number of mutexes used to protect rigid bodies. Zero selects the runtime default.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	uint32 NumBodyMutexes = 0;

	/// Maximum number of broad-phase body pairs queued at once.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	uint32 MaxBodyPairs = 65536;

	/// Maximum number of contact constraints retained by the physics world.
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	uint32 MaxContactConstraints = 10240;
	
	/**
	 * Default asset to be used for the physics layer of this project.
	 * It is possible to override the physics layer asset from the world settings directly.
	 */
	UPROPERTY(EditAnywhere, config, Category=JoltPhysics)
	TSoftObjectPtr<class UJPRPhysicsLayerDataAsset> DefaultLayer;

};
