// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "Physics/JPRPhysicsBody.h"

#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#endif // WITH_JOLT_PHYSICS

FJPRPhysicsBody::FJPRPhysicsBody() = default;

FJPRPhysicsBody::FJPRPhysicsBody(const FJPRPhysicsBody& Other) = default;

FJPRPhysicsBody& FJPRPhysicsBody::operator=(const FJPRPhysicsBody& Other) = default;

FJPRPhysicsBody::~FJPRPhysicsBody() = default;

bool FJPRPhysicsBody::IsEnabled() const
{
	return bEnabled;
}

bool FJPRPhysicsBody::DoesTriggerHitEvents() const
{
	return bTriggerHitEvents;
}

uint32 FJPRPhysicsBody::GetBodyID() const
{
#if WITH_JOLT_PHYSICS
	return body_id.IsValid() ? body_id->GetIndexAndSequenceNumber() : JPH::BodyID::cInvalidBodyID;
#else
	return 0;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetActive(bool bActive)
{
	bActive ? Activate() : Desactivate();
}

void FJPRPhysicsBody::Activate()
{
	if (bEnabled)
	{
		return;
	}
	bEnabled = true;
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		GetBodyInterface().AddBody(*body_id, JPH::EActivation::Activate);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::Desactivate()
{
	if (!bEnabled)
	{
		return;
	}
	bEnabled = false;
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		GetBodyInterface().RemoveBody(*body_id);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::ReleasePhysicsObject()
{
	Desactivate();
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		GetBodyInterface().DestroyBody(*body_id);
		body_id.Reset();
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetPosition(const FVector& Position) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsPosition = UnrealToJoltPhysics(Position);
		GetBodyInterface().SetPosition(*body_id, JPH::RVec3(PhysicsPosition.X, PhysicsPosition.Y, PhysicsPosition.Z), JPH::EActivation::DontActivate);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetRotation(const FQuat& Rotation) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FQuat PhysicsRotation = UnrealToJoltPhysics(Rotation);
		GetBodyInterface().SetRotation(*body_id, JPH::Quat(PhysicsRotation.X, PhysicsRotation.Y, PhysicsRotation.Z, PhysicsRotation.W), JPH::EActivation::DontActivate);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::GetPositionAndRotation(FVector& OutPosition, FQuat& OutRotation) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		JPH::RVec3 Position;
		JPH::Quat Rotation;
		GetBodyInterface().GetPositionAndRotation(*body_id, Position, Rotation);
		OutPosition = JoltPhysicsToUnreal(FVector(Position.GetX(), Position.GetY(), Position.GetZ()));
		OutRotation = JoltPhysicsToUnreal(FQuat(Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetPositionAndRotation(const FVector& Position, const FQuat& Rotation) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsPosition = UnrealToJoltPhysics(Position);
		const FQuat PhysicsRotation = UnrealToJoltPhysics(Rotation);
		GetBodyInterface().SetPositionAndRotation(*body_id, JPH::RVec3(PhysicsPosition.X, PhysicsPosition.Y, PhysicsPosition.Z),
			JPH::Quat(PhysicsRotation.X, PhysicsRotation.Y, PhysicsRotation.Z, PhysicsRotation.W), JPH::EActivation::DontActivate);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetWorldContextObject(UObject* Object)
{
	WorldContextObject = Object;
}

UObject* FJPRPhysicsBody::GetWorldContextObject() const
{
	return WorldContextObject.Get();
}

#if WITH_JOLT_PHYSICS
JPH::Body* FJPRPhysicsBody::CreateAndSetBody(const JPH::BodyCreationSettings& BodySettings, uint32 InBodyID)
{
	const JPH::BodyID NewBodyID(InBodyID);
	JPH::Body* Body = GetBodyInterface().CreateBodyWithID(NewBodyID, BodySettings);
	SetBodyID(NewBodyID);
	return Body;
}

void FJPRPhysicsBody::SetBodyID(const JPH::BodyID& InBodyID)
{
	body_id = MakeShared<JPH::BodyID>(InBodyID);
}

JPH::BodyInterface& FJPRPhysicsBody::GetBodyInterface() const
{
	check(PhysicsSystem.IsValid());
	return PhysicsSystem.Pin()->GetBodyInterface();
}

JPH::PhysicsSystem& FJPRPhysicsBody::GetPhysicsSystem() const
{
	check(PhysicsSystem.IsValid());
	return *PhysicsSystem.Pin();
}

JPH::TempAllocator& FJPRPhysicsBody::GetTempAllocator() const
{
	check(TempAllocator.IsValid());
	return *TempAllocator.Pin();
}
#endif // WITH_JOLT_PHYSICS
