// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Physics/JPRPhysicsTypes.h"

#ifndef WITH_JOLT_PHYSICS
#define WITH_JOLT_PHYSICS 0
#endif // WITH_JOLT_PHYSICS

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class Body;
	class BodyCreationSettings;
	class BodyID;
	class BodyInterface;
	class PhysicsSystem;
	class TempAllocator;
	class StateRecorder;
}
#endif // WITH_JOLT_PHYSICS

/**
 * Unreal-facing base for a body owned by the Jolt runtime wrapper.
 *
 * Jolt ownership and generic body behavior live here while Recall keeps its existing type as a
 * compatibility shell for Recall-specific behavior and serialization.
 */
class JOLTPHYSICSRUNTIME_API FJPRPhysicsBody
{
public:
	FJPRPhysicsBody();
	FJPRPhysicsBody(const FJPRPhysicsBody& Other);
	FJPRPhysicsBody& operator=(const FJPRPhysicsBody& Other);
	virtual ~FJPRPhysicsBody();

	bool IsEnabled() const;
	bool DoesTriggerHitEvents() const;
	uint32 GetBodyID() const;

	void SetActive(bool bActive);
	virtual void Activate();
	virtual void Desactivate();
	virtual void ReleasePhysicsObject();

	virtual void SetPosition(const FVector& Position) const;
	virtual void SetRotation(const FQuat& Rotation) const;
	virtual void GetPositionAndRotation(FVector& OutPosition, FQuat& OutRotation) const;
	virtual void SetPositionAndRotation(const FVector& Position, const FQuat& Rotation) const;
	void GetPosition(FVector& OutPosition) const;
	FTransform GetTransform() const;
	void SetRotation(const FRotator& Rotation) const;
	void GetRotation(FQuat& OutRotation) const;
	FVector GetForwardVector() const;
	FVector GetRightVector() const;
	
	virtual void AddLinearVelocityPerSecond(const FVector& LinearVelocity);
	virtual void AddLinearAndAngularVelocityPerSecond(const FVector& LinearVelocity, const FVector& AngularVelocityDegrees);
	virtual void SetLinearAndAngularVelocityPerSecond(const FVector& LinearVelocity, const FVector& AngularVelocityDegrees);
	virtual void SetLinearVelocityPerSecond(const FVector& LinearVelocity);
	virtual FVector GetLinearVelocityPerSecond() const;
	virtual void SetAngularVelocityPerSecond(const FVector& AngularVelocityDegrees);
	virtual FVector GetAngularVelocityPerSecond() const;

	bool CollideShape(const FVector& Position, uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const;
	bool ShapeCast(const FVector& Position, const FVector& Direction, float Distance,
		uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const;
	float GetMass() const;

	void SetWorldContextObject(UObject* Object);
	UObject* GetWorldContextObject() const;

	/**
	 * Draws a debug visualization of this body's shape in the world.
	 * @param World The world context for debug drawing
	 * @param Color Color to draw in
	 * @note Only available in non-shipping builds.
	 */
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const {}

	/**
	 * Draws debug information about this body (velocities, forces, etc).
	 * @param World The world context for debug drawing
	 * @param Color Color to draw in
	 * @note Only available in non-shipping builds.
	 */
	virtual void DrawDebugInfo(const UWorld* World, const FColor& Color) const {}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	virtual void DumpObject() const;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

#if WITH_JOLT_PHYSICS
	void SetPhysicsSystem(const TWeakPtr<JPH::PhysicsSystem>& System) { PhysicsSystemWeak = System; }
	void SetTempAllocator(const TWeakPtr<JPH::TempAllocator>& InTempAllocator) { TempAllocatorWeak = InTempAllocator; }

	virtual void SaveState(JPH::StateRecorder& State) {}
	virtual void RestoreState(JPH::StateRecorder& State) {}
	
	/**
	 * Helper method to save a FVector to the state recorder.
	 */
	static void SaveVector(JPH::StateRecorder& State, const FVector& Vec);
	
	/**
	 * Helper method to restore a FVector from the state recorder.
	 */
	static void RestoreVector(JPH::StateRecorder& State, FVector& Vec);
#endif // WITH_JOLT_PHYSICS
	
protected:
	bool bEnabled = false;
	bool bTriggerHitEvents = true;

#if WITH_JOLT_PHYSICS
	JPH::Body* CreateAndSetBody(const JPH::BodyCreationSettings& BodySettings, uint32 InBodyID);
	static void SetupBodyCreationSettings(JPH::BodyCreationSettings& BodyCreationSettings, const FJPRPhysicsBodyParameters& Params);
	void SetBodyID(const JPH::BodyID& InBodyID);

	JPH::BodyInterface& GetBodyInterface() const;
	JPH::PhysicsSystem& GetPhysicsSystem() const;
	JPH::TempAllocator& GetTempAllocator() const;

	TSharedPtr<JPH::BodyID> body_id;
#endif // WITH_JOLT_PHYSICS

private:
	TWeakObjectPtr<UObject> WorldContextObject;

#if WITH_JOLT_PHYSICS
	TWeakPtr<JPH::TempAllocator> TempAllocatorWeak;
	TWeakPtr<JPH::PhysicsSystem> PhysicsSystemWeak;
#endif // WITH_JOLT_PHYSICS
};
