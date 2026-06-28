// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "Physics/JPRPhysicsBody.h"

#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/PhysicsSystem.h>

using namespace JPH;
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

void FJPRPhysicsBody::GetPosition(FVector& OutPosition) const
{
	FQuat Rotation = FQuat::Identity;
	GetPositionAndRotation(OutPosition, Rotation);
}

FTransform FJPRPhysicsBody::GetTransform() const
{
	FVector Position = FVector::ZeroVector;
	FQuat Rotation = FQuat::Identity;
	GetPositionAndRotation(Position, Rotation);

	return FTransform(Rotation, Position);
}

void FJPRPhysicsBody::SetRotation(const FRotator& Rotation) const
{
	SetRotation(Rotation.Quaternion());
}

void FJPRPhysicsBody::GetRotation(FQuat& OutRotation) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const Quat Rotation = GetBodyInterface().GetRotation(*body_id.Get());
		OutRotation = JoltPhysicsToUnreal(FQuat(Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()));
	}
#endif // WITH_JOLT_PHYSICS
}

bool FJPRPhysicsBody::CollideShape(const FVector& Position, uint32& OutContactBodyID, FVector& OutContactPosition,
	FVector& OutContactNormal) const
{
#if WITH_JOLT_PHYSICS
	if (!body_id.IsValid())
	{
		return false;
	}
	
	RefConst<Shape> shape = GetBodyInterface().GetShape(*body_id.Get());

	const FVector inJoltRayOrigin = UnrealToJoltPhysics(Position);
	const RVec3 inOrigin(inJoltRayOrigin.X, inJoltRayOrigin.Y, inJoltRayOrigin.Z);
	
	const ObjectLayer object_layer = GetBodyInterface().GetObjectLayer(*body_id.Get());
	const DefaultBroadPhaseLayerFilter default_broadphase_layer_filter = GetPhysicsSystem().GetDefaultBroadPhaseLayerFilter(object_layer);
	const BroadPhaseLayerFilter& broadphase_layer_filter = default_broadphase_layer_filter;
	
	const DefaultObjectLayerFilter default_object_layer_filter = GetPhysicsSystem().GetDefaultLayerFilter(object_layer);
	const ObjectLayerFilter &object_layer_filter = default_object_layer_filter;

	const IgnoreSingleBodyFilter default_body_filter(*body_id.Get());
	const BodyFilter &body_filter = default_body_filter;
	
	RMat44 start_position = GetBodyInterface().GetWorldTransform(*body_id.Get());
	start_position.SetTranslation(inOrigin);
	
	class MyCollector : public CollideShapeCollector
	{
	public:
		MyCollector(PhysicsSystem &inPhysicsSystem) :
			mPhysicsSystem(inPhysicsSystem)
		{
		}

		virtual void		AddHit(const CollideShapeResult &inResult) override
		{
			// Test if this collision is closer than the previous one
			if (inResult.GetEarlyOutFraction() < GetEarlyOutFraction())
			{
				// Lock the body
				BodyLockRead lock(mPhysicsSystem.GetBodyLockInterfaceNoLock(), inResult.mBodyID2);
				JPH_ASSERT(lock.Succeeded()); // When this runs all bodies are locked so this should not fail
				const Body *body = &lock.GetBody();

				if (body->IsSensor())
					return;

				// Test that we're not hitting a vertical wall
				RVec3 contact_pos = inResult.mContactPointOn1;
				Vec3 normal = body->GetWorldSpaceSurfaceNormal(inResult.mSubShapeID2, contact_pos);
				
				// Update early out fraction to this hit
				UpdateEarlyOutFraction(inResult.GetEarlyOutFraction());

				// Get the contact properties
				mBody = body;
				mContactPosition = contact_pos;
				mContactNormal = normal;
			}
		}

		// Configuration
		PhysicsSystem &		mPhysicsSystem;

		// Resulting closest collision
		const Body *		mBody = nullptr;
		RVec3				mContactPosition = RVec3::sZero();
		Vec3				mContactNormal = Vec3::sZero();
	};

	CollideShapeSettings settings;

	MyCollector collector(GetPhysicsSystem());
	GetPhysicsSystem().GetNarrowPhaseQueryNoLock().CollideShape(shape, Vec3(1.0f, 1.0f, 1.0f), start_position,
		settings, shape->GetCenterOfMass(), collector, broadphase_layer_filter, object_layer_filter, body_filter);
	if (collector.mBody == nullptr)
	{
		return false;		
	}

	OutContactBodyID = collector.mBody->GetID().GetIndexAndSequenceNumber();
	OutContactPosition = JoltPhysicsToUnreal(
		FVector(collector.mContactPosition.GetX(), collector.mContactPosition.GetY(), collector.mContactPosition.GetZ()));
	OutContactNormal = JoltPhysicsToUnrealDirection(
		FVector(collector.mContactNormal.GetX(), collector.mContactNormal.GetY(), collector.mContactNormal.GetZ()));

	return true;
#else // WITH_JOLT_PHYSICS
	return false;
#endif // WITH_JOLT_PHYSICS
}

bool FJPRPhysicsBody::ShapeCast(const FVector& Position, const FVector& Direction, float Distance,
                                     uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const
{
#if WITH_JOLT_PHYSICS
	if (!body_id.IsValid())
	{
		return false;
	}
	
	RefConst<Shape> shape = GetBodyInterface().GetShape(*body_id.Get());

	const FVector inJoltRayOrigin = UnrealToJoltPhysics(Position);
	const RVec3 inOrigin(inJoltRayOrigin.X, inJoltRayOrigin.Y, inJoltRayOrigin.Z);
	
	const FVector inJoltRayDirection = UnrealToJoltPhysicsDirection(Direction);
	const Vec3 inDirection(inJoltRayDirection.X, inJoltRayDirection.Y, inJoltRayDirection.Z);

	const float ray_length = Distance * UnrealToJoltPhysicsUnitScale;

	const ObjectLayer object_layer = GetBodyInterface().GetObjectLayer(*body_id.Get());
	const DefaultBroadPhaseLayerFilter default_broadphase_layer_filter = GetPhysicsSystem().GetDefaultBroadPhaseLayerFilter(object_layer);
	const BroadPhaseLayerFilter& broadphase_layer_filter = default_broadphase_layer_filter;
	
	const DefaultObjectLayerFilter default_object_layer_filter = GetPhysicsSystem().GetDefaultLayerFilter(object_layer);
	const ObjectLayerFilter &object_layer_filter = default_object_layer_filter;

	const IgnoreSingleBodyFilter default_body_filter(*body_id.Get());
	// const BodyFilter &body_filter = mBodyFilter != nullptr? *mBodyFilter : default_body_filter;
	const BodyFilter &body_filter = default_body_filter;

	RMat44 start_position = GetBodyInterface().GetWorldTransform(*body_id.Get());
	start_position.SetTranslation(inOrigin);
	
	RShapeCast shape_cast(shape, Vec3::sReplicate(1.0f),
		start_position, inDirection * ray_length);

	class MyCollector : public CastShapeCollector
	{
	public:
		MyCollector(PhysicsSystem &inPhysicsSystem, const RShapeCast &inShape) :
			mPhysicsSystem(inPhysicsSystem),
			mShape(inShape)
		{
		}

		virtual void		AddHit(const ShapeCastResult &inResult) override
		{
			// Test if this collision is closer than the previous one
			if (inResult.mFraction < GetEarlyOutFraction())
			{
				// Lock the body
				BodyLockRead lock(mPhysicsSystem.GetBodyLockInterfaceNoLock(), inResult.mBodyID2);
				JPH_ASSERT(lock.Succeeded()); // When this runs all bodies are locked so this should not fail
				const Body *body = &lock.GetBody();

				if (body->IsSensor())
					return;

				// Test that we're not hitting a vertical wall
				RVec3 contact_pos = mShape.GetPointOnRay(inResult.mFraction);
				Vec3 normal = body->GetWorldSpaceSurfaceNormal(inResult.mSubShapeID2, contact_pos);
				
				// Update early out fraction to this hit
				UpdateEarlyOutFraction(inResult.mFraction);

				// Get the contact properties
				mBody = body;
				mContactPosition = contact_pos;
				mContactNormal = normal;
			}
		}

		// Configuration
		PhysicsSystem &		mPhysicsSystem;
		RShapeCast			mShape;

		// Resulting closest collision
		const Body *		mBody = nullptr;
		RVec3				mContactPosition = RVec3::sZero();
		Vec3				mContactNormal = Vec3::sZero();
	};

	ShapeCastSettings settings;

	MyCollector collector(GetPhysicsSystem(), shape_cast);
	GetPhysicsSystem().GetNarrowPhaseQueryNoLock().CastShape(shape_cast, settings, shape->GetCenterOfMass(),
		collector, broadphase_layer_filter, object_layer_filter, body_filter);
	if (collector.mBody == nullptr)
	{
		return false;		
	}
	
	OutContactBodyID = collector.mBody->GetID().GetIndexAndSequenceNumber();
	OutContactPosition = JoltPhysicsToUnreal(
		FVector(collector.mContactPosition.GetX(), collector.mContactPosition.GetY(), collector.mContactPosition.GetZ()));
	OutContactNormal = JoltPhysicsToUnrealDirection(
		FVector(collector.mContactNormal.GetX(), collector.mContactNormal.GetY(), collector.mContactNormal.GetZ()));

	return true;
#else // WITH_JOLT_PHYSICS
	return false;
#endif // WITH_JOLT_PHYSICS
}

float FJPRPhysicsBody::GetMass() const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		BodyLockRead lock(GetPhysicsSystem().GetBodyLockInterfaceNoLock(), *body_id.Get());
		JPH_ASSERT(lock.Succeeded()); // When this runs all bodies are locked so this should not fail
		const Body *body = &lock.GetBody();
		return body->GetBodyCreationSettings().GetMassProperties().mMass;
	}
#endif // WITH_JOLT_PHYSICS
	return 0.0f;
}

void FJPRPhysicsBody::AddLinearVelocityPerSecond(const FVector& LinearVelocity)
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsVelocity = UnrealToJoltPhysics(LinearVelocity);
		GetBodyInterface().AddLinearVelocity(*body_id, JPH::Vec3(PhysicsVelocity.X, PhysicsVelocity.Y, PhysicsVelocity.Z));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::AddLinearAndAngularVelocityPerSecond(const FVector& LinearVelocity, const FVector& AngularVelocityDegrees)
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsLinearVelocity = UnrealToJoltPhysics(LinearVelocity);
		const FVector PhysicsAngularVelocity = FMath::DegreesToRadians(UnrealToJoltPhysics(AngularVelocityDegrees));
		GetBodyInterface().AddLinearAndAngularVelocity(*body_id,
			JPH::Vec3(PhysicsLinearVelocity.X, PhysicsLinearVelocity.Y, PhysicsLinearVelocity.Z),
			JPH::Vec3(PhysicsAngularVelocity.X, PhysicsAngularVelocity.Y, PhysicsAngularVelocity.Z));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetLinearAndAngularVelocityPerSecond(const FVector& LinearVelocity, const FVector& AngularVelocityDegrees)
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsLinearVelocity = UnrealToJoltPhysics(LinearVelocity);
		const FVector PhysicsAngularVelocity = FMath::DegreesToRadians(UnrealToJoltPhysics(AngularVelocityDegrees));
		GetBodyInterface().SetLinearAndAngularVelocity(*body_id,
			JPH::Vec3(PhysicsLinearVelocity.X, PhysicsLinearVelocity.Y, PhysicsLinearVelocity.Z),
			JPH::Vec3(PhysicsAngularVelocity.X, PhysicsAngularVelocity.Y, PhysicsAngularVelocity.Z));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBody::SetLinearVelocityPerSecond(const FVector& LinearVelocity)
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsVelocity = UnrealToJoltPhysics(LinearVelocity);
		GetBodyInterface().SetLinearVelocity(*body_id, JPH::Vec3(PhysicsVelocity.X, PhysicsVelocity.Y, PhysicsVelocity.Z));
	}
#endif // WITH_JOLT_PHYSICS
}

FVector FJPRPhysicsBody::GetLinearVelocityPerSecond() const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const JPH::Vec3 PhysicsVelocity = GetBodyInterface().GetLinearVelocity(*body_id);
		return JoltPhysicsToUnreal(FVector(PhysicsVelocity.GetX(), PhysicsVelocity.GetY(), PhysicsVelocity.GetZ()));
	}
#endif // WITH_JOLT_PHYSICS
	return FVector::ZeroVector;
}

void FJPRPhysicsBody::SetAngularVelocityPerSecond(const FVector& AngularVelocityDegrees)
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const FVector PhysicsVelocity = FMath::DegreesToRadians(UnrealToJoltPhysics(AngularVelocityDegrees));
		GetBodyInterface().SetAngularVelocity(*body_id, JPH::Vec3(PhysicsVelocity.X, PhysicsVelocity.Y, PhysicsVelocity.Z));
	}
#endif // WITH_JOLT_PHYSICS
}

FVector FJPRPhysicsBody::GetAngularVelocityPerSecond() const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const JPH::Vec3 PhysicsVelocity = GetBodyInterface().GetAngularVelocity(*body_id);
		return FMath::RadiansToDegrees(JoltPhysicsToUnreal(
			FVector(PhysicsVelocity.GetX(), PhysicsVelocity.GetY(), PhysicsVelocity.GetZ())));
	}
#endif // WITH_JOLT_PHYSICS
	return FVector::ZeroVector;
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void FJPRPhysicsBody::DumpObject() const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		JPH::RVec3 PhysicsPosition = JPH::RVec3::sZero();
		JPH::Quat PhysicsRotation = JPH::Quat::sIdentity();
		GetBodyInterface().GetPositionAndRotation(*body_id, PhysicsPosition, PhysicsRotation);

		const FVector WorldPosition = JoltPhysicsToUnreal(
			FVector(PhysicsPosition.GetX(), PhysicsPosition.GetY(), PhysicsPosition.GetZ()));
		const JPH::Vec3 PhysicsVelocity = GetBodyInterface().GetLinearVelocity(*body_id);
		const FVector WorldVelocity = JoltPhysicsToUnreal(
			FVector(PhysicsVelocity.GetX(), PhysicsVelocity.GetY(), PhysicsVelocity.GetZ()));

		UE_LOG(LogTemp, Log, TEXT("[BodyID: %d]\nPhysicsVelocity: X=%3.3f Y=%3.3f Z=%3.3f\nWorldVelocity: %s\nPhysicsPosition: X=%3.3f Y=%3.3f Z=%3.3f\nWorldPosition: %s"),
			GetBodyID(), PhysicsVelocity.GetX(), PhysicsVelocity.GetY(), PhysicsVelocity.GetZ(), *WorldVelocity.ToString(),
			PhysicsPosition.GetX(), PhysicsPosition.GetY(), PhysicsPosition.GetZ(), *WorldPosition.ToString());
	}
#endif // WITH_JOLT_PHYSICS
}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

FVector FJPRPhysicsBody::GetForwardVector() const
{
	FQuat Rotation = FQuat::Identity;
	GetRotation(Rotation);
	return Rotation.GetForwardVector();
}

FVector FJPRPhysicsBody::GetRightVector() const
{
	FQuat Rotation = FQuat::Identity;
	GetRotation(Rotation);
	return Rotation.GetRightVector();
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

void FJPRPhysicsBody::SetupBodyCreationSettings(JPH::BodyCreationSettings& BodyCreationSettings, const FJPRPhysicsBodyParameters& Params)
{
	BodyCreationSettings.mGravityFactor = Params.GravityFactor;
	BodyCreationSettings.mMotionQuality = static_cast<JPH::EMotionQuality>(Params.MotionQuality);
	BodyCreationSettings.mEnhancedInternalEdgeRemoval = Params.EnhancedInternalEdgeRemoval;
	BodyCreationSettings.mFriction = Params.Friction;
	BodyCreationSettings.mAllowDynamicOrKinematic = Params.bAllowDynamicOrKinematic;
	BodyCreationSettings.mIsSensor = Params.bIsSensor;
	BodyCreationSettings.mRestitution = Params.Restitution;
	BodyCreationSettings.mMaxAngularVelocity = FMath::DegreesToRadians(Params.MaxAngularVelocityDegrees);
	BodyCreationSettings.mAllowSleeping = Params.bAllowSleeping;
	BodyCreationSettings.mAllowedDOFs = static_cast<JPH::EAllowedDOFs>(Params.AllowedDOFs);
	BodyCreationSettings.mOverrideMassProperties = static_cast<JPH::EOverrideMassProperties>(Params.OverrideMassProperties);
	BodyCreationSettings.mInertiaMultiplier = Params.InertiaMultiplier;
	BodyCreationSettings.mMassPropertiesOverride.mMass = Params.MassPropertiesOverride.Mass;
	BodyCreationSettings.mMassPropertiesOverride.mInertia = JPH::Mat44::sScale(Params.MassPropertiesOverride.Inertia);
	BodyCreationSettings.mNumVelocityStepsOverride = Params.NumVelocityStepsOverride;
	BodyCreationSettings.mNumPositionStepsOverride = Params.NumPositionStepsOverride;
}

void FJPRPhysicsBody::SetBodyID(const JPH::BodyID& InBodyID)
{
	body_id = MakeShared<JPH::BodyID>(InBodyID);
}

JPH::BodyInterface& FJPRPhysicsBody::GetBodyInterface() const
{
	check(PhysicsSystemWeak.IsValid());
	return PhysicsSystemWeak.Pin()->GetBodyInterface();
}

JPH::PhysicsSystem& FJPRPhysicsBody::GetPhysicsSystem() const
{
	check(PhysicsSystemWeak.IsValid());
	return *PhysicsSystemWeak.Pin();
}

JPH::TempAllocator& FJPRPhysicsBody::GetTempAllocator() const
{
	check(TempAllocatorWeak.IsValid());
	return *TempAllocatorWeak.Pin();
}

void FJPRPhysicsBody::SaveVector(JPH::StateRecorder& State, const FVector& Vec)
{
	State.Write(JPH::DVec3(Vec.X, Vec.Y, Vec.Z));
}

void FJPRPhysicsBody::RestoreVector(JPH::StateRecorder& State, FVector& Vec)
{
	JPH::DVec3 Src = JPH::DVec3::sZero();
	State.Read(Src);
	Vec = FVector(Src.GetX(), Src.GetY(), Src.GetZ());
}

#endif // WITH_JOLT_PHYSICS
