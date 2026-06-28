// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "JPRPhysicsSubsystem.generated.h"

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class BodyInterface;
	class BodyActivationListener;
	class ContactListener;
	class BroadPhaseLayerInterface;
	class JobSystemThreadPool;
	class ObjectLayerPairFilter;
	class ObjectVsBroadPhaseLayerFilter;
	class PhysicsSystem;
	class StateRecorderFilter;
	class TempAllocator;
}
#endif // WITH_JOLT_PHYSICS

class UJPRPhysicsLayerDataAsset;

/**
 * Unreal-facing owner for a Jolt physics world.
 *
 * World creation, ticking, contacts, and state serialization will move into this base incrementally.
 * Recall derives from this type so existing subsystem references remain stable throughout the migration.
 */
UCLASS(Abstract)
class JOLTPHYSICSRUNTIME_API UJPRPhysicsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
#if WITH_JOLT_PHYSICS
	JPH::PhysicsSystem& GetPhysicsSystem() const;
	JPH::BodyInterface& GetBodyInterface() const;
	TSharedPtr<JPH::PhysicsSystem> GetPhysicsSystemPtr() const { return PhysicsSystem; }
	TSharedPtr<JPH::TempAllocator> GetTempAllocator() const { return TempAllocator; }
#endif // WITH_JOLT_PHYSICS

protected:
	void StepPhysics(float DeltaTime, int32 CollisionSteps, float TimeDilation = 1.0f);

#if WITH_JOLT_PHYSICS
	void CreatePhysicsSystem(const UJPRPhysicsLayerDataAsset& PhysicsLayer, JPH::BodyActivationListener* BodyActivationListener,
		JPH::ContactListener* ContactListener, const TSharedPtr<JPH::StateRecorderFilter>& InStateRecorderFilter);
#endif // WITH_JOLT_PHYSICS
	void DeletePhysicsSystem();

#if WITH_JOLT_PHYSICS
	TSharedPtr<JPH::TempAllocator> TempAllocator;
	TSharedPtr<JPH::JobSystemThreadPool> JobSystem;
	TSharedPtr<JPH::PhysicsSystem> PhysicsSystem;
	TSharedPtr<JPH::BroadPhaseLayerInterface> BroadPhaseLayerInterface;
	TSharedPtr<JPH::ObjectVsBroadPhaseLayerFilter> ObjectVsBroadPhaseLayerFilter;
	TSharedPtr<JPH::ObjectLayerPairFilter> ObjectLayerPairFilter;
	TSharedPtr<JPH::StateRecorderFilter> StateRecorderFilter;
#endif // WITH_JOLT_PHYSICS
};
