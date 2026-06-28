// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "JPRPhysicsCharacterObject.h"

#include "DrawDebugHelpers.h"
#include "JPRPhysicsCharacterShapeTypes.h"
#include "Physics/JPRPhysicsTypes.h"
#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Character/Character.h>
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

// FJPRPhysicsCharacterBody Begin
void FJPRPhysicsCharacterBody::Activate()
{
	FJPRPhysicsBody::Activate();
}

void FJPRPhysicsCharacterBody::Desactivate()
{
	FJPRPhysicsBody::Desactivate();
}

void FJPRPhysicsCharacterBody::ReleasePhysicsObject()
{
	Desactivate();

#if WITH_JOLT_PHYSICS
	character.Reset();
	body_id.Reset();
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCharacterBody::PostSimulation()
{
#if WITH_JOLT_PHYSICS
	if (character.IsValid())
	{
		character->PostSimulation(MaxSeparationDistance);
	}
#endif // WITH_JOLT_PHYSICS
}

#if WITH_JOLT_PHYSICS
void FJPRPhysicsCharacterBody::SaveState(StateRecorder& State)
{
	if (character.IsValid())
	{
		character->SaveState(State);
	}
}

void FJPRPhysicsCharacterBody::RestoreState(StateRecorder& State)
{
	if (character.IsValid())
	{
		character->RestoreState(State);
	}
}
#endif // WITH_JOLT_PHYSICS

void FJPRPhysicsCharacterBody::InitCharacter(const FJPRPhysicsCharacter& CharacterShape,
	const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 InLayer)
{
#if WITH_JOLT_PHYSICS
	const TSharedPtr<BodyID> NewBodyID(new BodyID(InBodyID));

	const float Radius = CharacterShape.Capsule.Radius * UnrealToJoltPhysicsUnitScale;
	const float HalfHeightOfCylinder = CharacterShape.Capsule.HalfHeightOfCylinder * UnrealToJoltPhysicsUnitScale;
	
	JPH::Ref<CharacterSettings> settings = new CharacterSettings();
	settings->mMaxSlopeAngle = DegreesToRadians(45.0f);
	settings->mLayer = static_cast<ObjectLayer>(InLayer);
	settings->mShape = new CapsuleShape(HalfHeightOfCylinder, Radius);
	settings->mFriction = Params.Friction;
	settings->mGravityFactor = Params.GravityFactor;
	settings->mMotionQuality = static_cast<EMotionQuality>(Params.MotionQuality);
	settings->mAllowSleeping = Params.bAllowSleeping;

	character = MakeShared<Character>(settings, RVec3::sZero(), Quat::sIdentity(), 0, &GetPhysicsSystem(), NewBodyID.Get());

	SetBodyID(*NewBodyID.Get());

	bTriggerHitEvents = Params.bTriggerHitEvents;
	bUseGroundState = CharacterShape.bUseGroundState;
	MaxSeparationDistance = CharacterShape.MaxSeparationDistance * UnrealToJoltPhysicsUnitScale;
#endif // WITH_JOLT_PHYSICS
}

bool FJPRPhysicsCharacterBody::IsSupported() const
{
#if WITH_JOLT_PHYSICS
	if (bUseGroundState && character.IsValid())
	{
		return character->IsSupported();
	}
#endif // WITH_JOLT_PHYSICS
	
	return true;
}

EJPRPhysicsCharacterGroundState FJPRPhysicsCharacterBody::GetGroundState() const
{
#if WITH_JOLT_PHYSICS
	if (bUseGroundState && character.IsValid())
	{
		return static_cast<EJPRPhysicsCharacterGroundState>(character->GetGroundState());
	}
#endif // WITH_JOLT_PHYSICS
	
	return EJPRPhysicsCharacterGroundState::OnGround;
}

void FJPRPhysicsCharacterBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
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
// FJPRPhysicsCharacterBody End
