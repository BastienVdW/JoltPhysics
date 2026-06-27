// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");

#include "Physics/Collision/JPRPhysicsGroundCollisionTesterRay.h"

#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>

JPH_SUPPRESS_WARNINGS

using namespace JPH;
#endif // WITH_JOLT_PHYSICS

FJPRPhysicsGroundCollisionTesterRay::FJPRPhysicsGroundCollisionTesterRay(int32 InObjectLayer, FVector InUp, float InMaxSlopeAngle)
	: ObjectLayer(InObjectLayer)
	, Up(UnrealToJoltPhysicsDirection(InUp))
	, CosMaxSlopeAngle(FMath::Cos(FMath::DegreesToRadians(InMaxSlopeAngle)))
{
}

#if WITH_JOLT_PHYSICS
bool FJPRPhysicsGroundCollisionTesterRay::Collide(PhysicsSystem& InPhysicsSystem, uint32 InBodyID,
	const FVector& InRayOrigin, const FVector& InRayDirection, float InRayLength,
	uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const
{
	const FVector JoltRayOrigin = UnrealToJoltPhysics(InRayOrigin);
	const Vec3 Origin(JoltRayOrigin.X, JoltRayOrigin.Y, JoltRayOrigin.Z);
	const FVector JoltRayDirection = UnrealToJoltPhysicsDirection(InRayDirection);
	const Vec3 Direction(JoltRayDirection.X, JoltRayDirection.Y, JoltRayDirection.Z);
	const float RayLength = InRayLength * UnrealToJoltPhysicsUnitScale;
	const JPH::ObjectLayer JoltObjectLayer(ObjectLayer);

	const DefaultBroadPhaseLayerFilter BroadPhaseLayerFilter = InPhysicsSystem.GetDefaultBroadPhaseLayerFilter(JoltObjectLayer);
	const DefaultObjectLayerFilter ObjectLayerFilter = InPhysicsSystem.GetDefaultLayerFilter(JoltObjectLayer);
	const IgnoreSingleBodyFilter BodyFilter{ BodyID(InBodyID) };
	const RRayCast Ray{ Origin, RayLength * Direction };

	class FCollector final : public CastRayCollector
	{
	public:
		FCollector(PhysicsSystem& InSystem, const RRayCast& InRay, Vec3Arg InUp, float InCosMaxSlopeAngle)
			: System(InSystem), Ray(InRay), UpDirection(InUp), CosMaxSlopeAngle(InCosMaxSlopeAngle)
		{
		}

		virtual void AddHit(const RayCastResult& Result) override
		{
			if (Result.mFraction >= GetEarlyOutFraction())
			{
				return;
			}

			BodyLockRead Lock(System.GetBodyLockInterfaceNoLock(), Result.mBodyID);
			JPH_ASSERT(Lock.Succeeded());
			const Body* HitBody = &Lock.GetBody();
			if (HitBody->IsSensor() || !HitBody->IsStatic())
			{
				return;
			}

			const RVec3 Position = Ray.GetPointOnRay(Result.mFraction);
			const Vec3 Normal = HitBody->GetWorldSpaceSurfaceNormal(Result.mSubShapeID2, Position);
			if (Normal.Dot(UpDirection) <= CosMaxSlopeAngle)
			{
				return;
			}

			UpdateEarlyOutFraction(Result.mFraction);
			HitBodyResult = HitBody;
			ContactPosition = Position;
			ContactNormal = Normal;
		}

		PhysicsSystem& System;
		RRayCast Ray;
		Vec3 UpDirection;
		float CosMaxSlopeAngle;
		const JPH::Body* HitBodyResult = nullptr;
		RVec3 ContactPosition = RVec3::sZero();
		Vec3 ContactNormal = Vec3::sZero();
	};

	RayCastSettings Settings;
	FCollector Collector(InPhysicsSystem, Ray, Vec3(Up.X, Up.Y, Up.Z), CosMaxSlopeAngle);
	InPhysicsSystem.GetNarrowPhaseQueryNoLock().CastRay(
		Ray, Settings, Collector, BroadPhaseLayerFilter, ObjectLayerFilter, BodyFilter);
	if (Collector.HitBodyResult == nullptr)
	{
		return false;
	}

	OutContactBodyID = Collector.HitBodyResult->GetID().GetIndexAndSequenceNumber();
	OutContactPosition = JoltPhysicsToUnreal(FVector(
		Collector.ContactPosition.GetX(), Collector.ContactPosition.GetY(), Collector.ContactPosition.GetZ()));
	OutContactNormal = JoltPhysicsToUnrealDirection(FVector(
		Collector.ContactNormal.GetX(), Collector.ContactNormal.GetY(), Collector.ContactNormal.GetZ()));
	return true;
}
#endif // WITH_JOLT_PHYSICS
