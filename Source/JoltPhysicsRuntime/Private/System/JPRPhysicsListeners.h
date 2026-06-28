// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "System/JPRPhysicsSubsystem.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/StateRecorder.h>

class FJPRBodyActivationListener final : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JPH::BodyID& BodyID, JPH::uint64 BodyUserData) override {}
	virtual void OnBodyDeactivated(const JPH::BodyID& BodyID, JPH::uint64 BodyUserData) override {}
};

class FJPRContactListener final : public JPH::ContactListener
{
public:
	virtual ValidateResult OnContactValidate(const JPH::Body& Body1, const JPH::Body& Body2, JPH::RVec3Arg BaseOffset,
		const JPH::CollideShapeResult& CollisionResult) override;
	virtual void OnContactAdded(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold,
		JPH::ContactSettings& Settings) override;
	virtual void OnContactPersisted(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold,
		JPH::ContactSettings& Settings) override;

	TArray<FJPRContactEvent> ConsumeEvents();
	int32 GetNumEvents() const;

private:
	void AddEvent(const JPH::Body& Body1, const JPH::Body& Body2, const JPH::ContactManifold& Manifold);

	mutable FCriticalSection DataGuard;
	TArray<FJPRContactEvent> Events;
};

class FJPRStateRecorderFilter final : public JPH::StateRecorderFilter
{
public:
	explicit FJPRStateRecorderFilter(TFunction<bool(uint32)> InShouldSaveBody)
		: ShouldSaveBodyPredicate(MoveTemp(InShouldSaveBody))
	{
	}

	virtual bool ShouldSaveBody(const JPH::Body& Body) const override;

private:
	TFunction<bool(uint32)> ShouldSaveBodyPredicate;
};
#endif // WITH_JOLT_PHYSICS
