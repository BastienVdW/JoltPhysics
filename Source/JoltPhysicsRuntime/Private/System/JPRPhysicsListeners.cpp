// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "System/JPRPhysicsListeners.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Physics/Body/Body.h>

JPH::ValidateResult FJPRContactListener::OnContactValidate(const JPH::Body& Body1, const JPH::Body& Body2,

	const JPH::RVec3Arg BaseOffset, const JPH::CollideShapeResult& CollisionResult)
{
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void FJPRContactListener::OnContactAdded(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold,
	JPH::ContactSettings& Settings)
{
	AddEvent(Body1, Body2, Manifold);
}

void FJPRContactListener::OnContactPersisted(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold,
	JPH::ContactSettings& Settings)
{
	AddEvent(Body1, Body2, Manifold);
}

void FJPRContactListener::AddEvent(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold)
{
	FJPRContactEvent Event;
	Event.BodyID1 = Body1.GetID().GetIndexAndSequenceNumber();
	Event.BodyID2 = Body2.GetID().GetIndexAndSequenceNumber();
	Event.Velocity1 = FVector(Body1.GetLinearVelocity().GetX(), Body1.GetLinearVelocity().GetY(), Body1.GetLinearVelocity().GetZ());
	Event.Velocity2 = FVector(Body2.GetLinearVelocity().GetX(), Body2.GetLinearVelocity().GetY(), Body2.GetLinearVelocity().GetZ());
	Event.bIsStatic1 = Body1.IsStatic();
	Event.bIsStatic2 = Body2.IsStatic();
	Event.bIsSensor1 = Body1.IsSensor();
	Event.bIsSensor2 = Body2.IsSensor();
	const JPH::RVec3 Point1 = Manifold.GetWorldSpaceContactPointOn1(0);
	const JPH::RVec3 Point2 = Manifold.GetWorldSpaceContactPointOn2(0);
	Event.ImpactPoint1 = FVector(Point1.GetX(), Point1.GetY(), Point1.GetZ());
	Event.ImpactPoint2 = FVector(Point2.GetX(), Point2.GetY(), Point2.GetZ());
	Event.ImpactNormal1 = FVector(Manifold.mWorldSpaceNormal.GetX(), Manifold.mWorldSpaceNormal.GetY(), Manifold.mWorldSpaceNormal.GetZ());
	const JPH::Vec3 Normal2 = Manifold.SwapShapes().mWorldSpaceNormal;
	Event.ImpactNormal2 = FVector(Normal2.GetX(), Normal2.GetY(), Normal2.GetZ());
	Event.SubShapeID1 = Manifold.mSubShapeID1.GetValue();
	Event.SubShapeID2 = Manifold.mSubShapeID2.GetValue();

	FScopeLock Lock(&DataGuard);
	Events.Add(MoveTemp(Event));
}

TArray<FJPRContactEvent> FJPRContactListener::ConsumeEvents()
{
	FScopeLock Lock(&DataGuard);
	TArray<FJPRContactEvent> Result = MoveTemp(Events);
	Events.Reset();
	return Result;
}

int32 FJPRContactListener::GetNumEvents() const
{
	FScopeLock Lock(&DataGuard);
	return Events.Num();
}

bool FJPRStateRecorderFilter::ShouldSaveBody(const JPH::Body& Body) const
{
	if (Body.IsStatic() && !Body.CanBeKinematicOrDynamic())
	{
		return ShouldSaveBodyPredicate && ShouldSaveBodyPredicate(Body.GetID().GetIndexAndSequenceNumber());
	}
	return true;
}
#endif // WITH_JOLT_PHYSICS
