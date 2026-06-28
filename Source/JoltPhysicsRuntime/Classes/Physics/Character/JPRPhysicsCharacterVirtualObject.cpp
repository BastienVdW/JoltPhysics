// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "JPRPhysicsCharacterVirtualObject.h"

#include "DrawDebugHelpers.h"
#include "JPRPhysicsCharacterShapeTypes.h"
#include "Physics/JPRPhysicsTypes.h"
#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;
#endif // WITH_JOLT_PHYSICS

struct FJPRPhysicsCharacterVirtualBodyShapeContainer
{
#if WITH_JOLT_PHYSICS
	RefConst<JPH::CapsuleShape> standing_capsule_shape;
	RefConst<JPH::CapsuleShape> standing_inner_capsule_shape;
	RefConst<JPH::CapsuleShape> crouching_capsule_shape;
	RefConst<JPH::CapsuleShape> crouching_inner_capsule_shape;
#endif // WITH_JOLT_PHYSICS
};

// FJPRPhysicsCharacterVirtualBody Begin
void FJPRPhysicsCharacterVirtualBody::Activate()
{
	FJPRPhysicsBody::Activate();
}

void FJPRPhysicsCharacterVirtualBody::Desactivate()
{
	FJPRPhysicsBody::Desactivate();
}

void FJPRPhysicsCharacterVirtualBody::AddLinearVelocityPerSecond(const FVector& LinearVelocity)
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const FVector PhysicsVelocity = UnrealToJoltPhysics(LinearVelocity);
		const Vec3 Vel(PhysicsVelocity.X, PhysicsVelocity.Y, PhysicsVelocity.Z);

		character->SetLinearVelocity(character->GetLinearVelocity() + Vel);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::SetLinearVelocityPerSecond(const FVector& LinearVelocity)
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const FVector PhysicsVelocity = UnrealToJoltPhysics(LinearVelocity);
		const Vec3 Vel(PhysicsVelocity.X, PhysicsVelocity.Y, PhysicsVelocity.Z);

		character->SetLinearVelocity(Vel);
	}
#endif // WITH_JOLT_PHYSICS
}

FVector FJPRPhysicsCharacterVirtualBody::GetLinearVelocityPerSecond() const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const Vec3 PhysicsVelocity = character->GetLinearVelocity();
		const FVector WorldVelocity = JoltPhysicsToUnreal(FVector(PhysicsVelocity.GetX(), PhysicsVelocity.GetY(), PhysicsVelocity.GetZ()));

		return WorldVelocity;
	}
#endif // WITH_JOLT_PHYSICS

	return FJPRPhysicsBody::GetLinearVelocityPerSecond();
}

void FJPRPhysicsCharacterVirtualBody::SetPosition(const FVector& Position) const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const FVector PhysicsPos = UnrealToJoltPhysics(Position);
		const RVec3 Pos(PhysicsPos.X, PhysicsPos.Y, PhysicsPos.Z);

		character->SetPosition(Pos);
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::SetRotation(const FQuat& Rotation) const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const FQuat Rot = UnrealToJoltPhysics(Rotation);
		character->SetRotation(Quat(Rot.X, Rot.Y, Rot.Z, Rot.W));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::SetPositionAndRotation(const FVector& Position, const FQuat& Rotation) const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const FVector PhysicsPos = UnrealToJoltPhysics(Position);
		const RVec3 Pos(PhysicsPos.X, PhysicsPos.Y, PhysicsPos.Z);
		const FQuat Rot = UnrealToJoltPhysics(Rotation);

		character->SetPosition(Pos);
		character->SetRotation(Quat(Rot.X, Rot.Y, Rot.Z, Rot.W));
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::GetPositionAndRotation(FVector& OutPosition, FQuat& OutRotation) const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		RVec3 Position = character->GetPosition();
		Quat Rotation = character->GetRotation();
		
		OutPosition = JoltPhysicsToUnreal(FVector(Position.GetX(), Position.GetY(), Position.GetZ()));
		OutRotation = JoltPhysicsToUnreal(FQuat(Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()));
		return;
	}
#endif // WITH_JOLT_PHYSICS
	
	FJPRPhysicsBody::GetPositionAndRotation(OutPosition, OutRotation);
}

void FJPRPhysicsCharacterVirtualBody::ReleasePhysicsObject()
{
	Desactivate();

#if WITH_JOLT_PHYSICS
	character.Reset();
	body_id.Reset();
#endif // WITH_JOLT_PHYSICS
}

#if WITH_JOLT_PHYSICS
void FJPRPhysicsCharacterVirtualBody::SaveState(StateRecorder& State)
{
	if (character.IsValid())
	{
		character->SaveState(State);
	}

	State.Write(bIsStanding);
}

void FJPRPhysicsCharacterVirtualBody::RestoreState(StateRecorder& State)
{
	if (character.IsValid())
	{
		character->RestoreState(State);
	}

	State.Read(bIsStanding);
	SwitchStance(bIsStanding, FLT_MAX);
}
#endif // WITH_JOLT_PHYSICS

void FJPRPhysicsCharacterVirtualBody::InitCharacterVirtual(const FJPRPhysicsCharacterVirtual& CharacterVirtualShape,
                                                                const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 InLayer)
{
#if WITH_JOLT_PHYSICS
	const TSharedPtr<BodyID> NewBodyID(new BodyID(InBodyID));

	const float StandingRadius = CharacterVirtualShape.StandingCapsule.Radius * UnrealToJoltPhysicsUnitScale;
	const float StandingHalfHeightOfCylinder = CharacterVirtualShape.StandingCapsule.HalfHeightOfCylinder * UnrealToJoltPhysicsUnitScale;
	
	const float CrouchingRadius = CharacterVirtualShape.CrouchingCapsule.Radius * UnrealToJoltPhysicsUnitScale;
	const float CrouchingHalfHeightOfCylinder = CharacterVirtualShape.CrouchingCapsule.HalfHeightOfCylinder * UnrealToJoltPhysicsUnitScale;

	StandOffset = FMath::Max(0.0f, StandingHalfHeightOfCylinder - CrouchingHalfHeightOfCylinder);
	
	const float InnerShapeFraction = CharacterVirtualShape.InnerShapeFraction;

	ShapeContainer = MakeShared<FJPRPhysicsCharacterVirtualBodyShapeContainer>();

	ShapeContainer->standing_capsule_shape = new CapsuleShape(StandingHalfHeightOfCylinder, StandingRadius);
	ShapeContainer->standing_inner_capsule_shape = new CapsuleShape(
		StandingHalfHeightOfCylinder * InnerShapeFraction, StandingRadius * InnerShapeFraction);
	
	ShapeContainer->crouching_capsule_shape = new CapsuleShape(CrouchingHalfHeightOfCylinder, CrouchingRadius);
	ShapeContainer->crouching_inner_capsule_shape = new CapsuleShape(
		CrouchingHalfHeightOfCylinder * InnerShapeFraction, CrouchingRadius * InnerShapeFraction);
		
	JPH::Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
	settings->mMaxSlopeAngle = DegreesToRadians(45.0f);
	settings->mShape = ShapeContainer->standing_capsule_shape;

	// The inner shape can be changed while crouching
	settings->mInnerBodyLayer = static_cast<ObjectLayer>(InLayer);
	settings->mInnerBodyShape = ShapeContainer->standing_inner_capsule_shape;
		
	// settings->mFriction = Params.Friction;
	// settings->mGravityFactor = Params.GravityFactor;
	// settings->mMotionQuality = static_cast<EMotionQuality>(Params.MotionQuality);
	// settings->mAllowSleeping = Params.bAllowSleeping;
		
	character = MakeShared<CharacterVirtual>(settings, RVec3::sZero(), Quat::sIdentity(), 0, &GetPhysicsSystem(), NewBodyID.Get());	

	SetBodyID(*NewBodyID.Get());

	bTriggerHitEvents = Params.bTriggerHitEvents;
	Layer = InLayer;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::Update(float DeltaTime)
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const PhysicsSystem& PhysicsSystem = GetPhysicsSystem();
		const Vec3 gravity = PhysicsSystem.GetGravity();
		
		Vec3 new_velocity = character->GetLinearVelocity();

		// Apply gravity
		if (character->GetGroundState() == CharacterBase::EGroundState::OnGround)
		{
			new_velocity.SetY(FMath::Max(0.0, new_velocity.GetY()));
		}
		else
		{			
			new_velocity += gravity * DeltaTime;
		}

		character->SetLinearVelocity(new_velocity);

		CharacterVirtual::ExtendedUpdateSettings update_settings;
		character->ExtendedUpdate(DeltaTime, gravity,
			update_settings,
			PhysicsSystem.GetDefaultBroadPhaseLayerFilter(static_cast<ObjectLayer>(Layer)),
			PhysicsSystem.GetDefaultLayerFilter(static_cast<ObjectLayer>(Layer)),
			{ }, { }, GetTempAllocator());
	}
#endif // WITH_JOLT_PHYSICS
}

bool FJPRPhysicsCharacterVirtualBody::IsSupported() const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		return character->IsSupported();
	}
#endif // WITH_JOLT_PHYSICS
	
	return true;
}

EJPRPhysicsCharacterGroundState FJPRPhysicsCharacterVirtualBody::GetGroundState() const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		return static_cast<EJPRPhysicsCharacterGroundState>(character->GetGroundState());
	}
#endif // WITH_JOLT_PHYSICS
	
	return EJPRPhysicsCharacterGroundState::OnGround;
}

bool FJPRPhysicsCharacterVirtualBody::SetStance(bool bStand)
{
	if (bIsStanding == bStand)
	{
		return false;
	}

	const PhysicsSystem& PhysicsSystem = GetPhysicsSystem();
	const float PenetrationSlop = PhysicsSystem.GetPhysicsSettings().mPenetrationSlop;
	const float Offset = bStand ? StandOffset : 0.0f;	
	
	if (!SwitchStance(bStand, PenetrationSlop, Offset))
	{
		return false;
	}
	
	bIsStanding = bStand;
	return true;
}

bool FJPRPhysicsCharacterVirtualBody::SwitchStance(bool bStand, float MaxPenetrationDepth, float Offset)
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const Shape* shape = bStand ? ShapeContainer->standing_capsule_shape : ShapeContainer->crouching_capsule_shape;
		const Shape* inner_shape = bStand ? ShapeContainer->standing_inner_capsule_shape : ShapeContainer->crouching_inner_capsule_shape;
		
		const PhysicsSystem& PhysicsSystem = GetPhysicsSystem();
		if (character->SetShape(shape, Offset, MaxPenetrationDepth, PhysicsSystem.GetDefaultBroadPhaseLayerFilter(Layer),
			PhysicsSystem.GetDefaultLayerFilter(Layer), { }, { }, GetTempAllocator()))
		{			
			character->SetInnerBodyShape(inner_shape);
			return true;
		}
	}
	return false;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterVirtualBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		const CapsuleShape* Shape = static_cast<const CapsuleShape*>(character->GetShape());

		const float HalfHeight = (Shape->GetHalfHeightOfCylinder() + Shape->GetRadius()) * JoltPhysicsToUnrealUnitScale;
		const float Radius = Shape->GetRadius() * JoltPhysicsToUnrealUnitScale;

		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		DrawDebugCapsule(World, WorldPosition, HalfHeight, Radius, WorldRotation, Color);
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsCharacterVirtualBody End
