// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

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

	bool CollideShape(const FVector& Position, uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const;
	bool ShapeCast(const FVector& Position, const FVector& Direction, float Distance,
		uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const;
	float GetMass() const;

	void SetWorldContextObject(UObject* Object);
	UObject* GetWorldContextObject() const;

#if WITH_JOLT_PHYSICS
	void SetPhysicsSystem(const TWeakPtr<JPH::PhysicsSystem>& System) { PhysicsSystemWeak = System; }
	void SetTempAllocator(const TWeakPtr<JPH::TempAllocator>& InTempAllocator) { TempAllocatorWeak = InTempAllocator; }
#endif // WITH_JOLT_PHYSICS

protected:
	bool bEnabled = false;
	bool bTriggerHitEvents = true;

#if WITH_JOLT_PHYSICS
	JPH::Body* CreateAndSetBody(const JPH::BodyCreationSettings& BodySettings, uint32 InBodyID);
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
