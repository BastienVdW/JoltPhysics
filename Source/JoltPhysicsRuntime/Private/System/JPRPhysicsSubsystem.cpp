// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "System/JPRPhysicsSubsystem.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "Physics/JPRPhysicsLayerFilters.h"
#include "Physics/JPRPhysicsBody.h"
#include "Settings/JPRPhysicsSettings.h"
#include "System/JPRPhysicsListeners.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyLockMulti.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/StateRecorderImpl.h>

#include <cstdarg>
#endif // WITH_JOLT_PHYSICS

#if WITH_JOLT_PHYSICS
namespace
{
	void TraceImpl(const char* Format, ...)
	{
		va_list Arguments;
		va_start(Arguments, Format);
		char Buffer[1024];
		vsnprintf(Buffer, sizeof(Buffer), Format, Arguments);
		va_end(Arguments);

		UE_LOG(LogTemp, Log, TEXT("%s"), UTF8_TO_TCHAR(Buffer));
	}

#ifdef JPH_ENABLE_ASSERTS
	bool AssertFailedImpl(const char* Expression, const char* Message, const char* File, const JPH::uint Line)
	{
		UE_LOG(LogTemp, Error, TEXT("%s:%s: (%d) %s"), UTF8_TO_TCHAR(Expression), UTF8_TO_TCHAR(File), Line, UTF8_TO_TCHAR(Message));
		UE_DEBUG_BREAK();
		return true;
	}
#endif // JPH_ENABLE_ASSERTS
}
#endif // WITH_JOLT_PHYSICS

void UJPRPhysicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UJPRPhysicsSettings* PhysicsSettings = GetDefault<UJPRPhysicsSettings>();
	PhysicsLayer = PhysicsSettings->DefaultLayer.LoadSynchronous();

	if (PhysicsLayer)
	{
		CreatePhysicsSystem();
	}
}

void UJPRPhysicsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	PhysicsLayer = nullptr;
	
	DeletePhysicsSystem();
}

void UJPRPhysicsSubsystem::StepPhysics(const float DeltaTime, const int32 CollisionSteps, const float TimeDilation)
{
#if WITH_JOLT_PHYSICS
	check(PhysicsSystem.IsValid());
	check(TempAllocator.IsValid());
	check(JobSystem.IsValid());
	PhysicsSystem->Update(DeltaTime * TimeDilation, CollisionSteps, TempAllocator.Get(), JobSystem.Get());
	JobSystem->WaitThreads();
#endif // WITH_JOLT_PHYSICS
}

bool UJPRPhysicsSubsystem::HasPhysicsSystem() const
{
#if WITH_JOLT_PHYSICS
	return PhysicsSystem.IsValid();
#else // WITH_JOLT_PHYSICS
	return false;
#endif
}

TArray<uint8> UJPRPhysicsSubsystem::SavePhysicsState(const TArray<TSharedPtr<FJPRPhysicsBody>>& Bodies) const
{
	TArray<uint8> Result;
#if WITH_JOLT_PHYSICS
	JPH::StateRecorderImpl State;
	GetPhysicsSystem().SaveState(State, JPH::EStateRecorderState::All, StateRecorderFilter.Get());
	for (const TSharedPtr<FJPRPhysicsBody>& Body : Bodies)
	{
		if (ensure(Body.IsValid()))
		{
			Body->SaveState(State);
		}
	}

	const std::string& Data = State.GetData();
	Result.Append(reinterpret_cast<const uint8*>(Data.data()), Data.size());
#endif // WITH_JOLT_PHYSICS
	return Result;
}

bool UJPRPhysicsSubsystem::RestorePhysicsState(const TArray<uint8>& StateData, const TArray<TSharedPtr<FJPRPhysicsBody>>& Bodies) const
{
#if WITH_JOLT_PHYSICS
	JPH::StateRecorderImpl State;
	State.WriteBytes(StateData.GetData(), StateData.Num());
	State.Rewind();
	if (!ensure(GetPhysicsSystem().RestoreState(State, StateRecorderFilter.Get())))
	{
		return false;
	}

	for (const TSharedPtr<FJPRPhysicsBody>& Body : Bodies)
	{
		if (ensure(Body.IsValid()))
		{
			Body->RestoreState(State);
		}
	}
	return true;
#else // WITH_JOLT_PHYSICS
	return false;
#endif
}

void UJPRPhysicsSubsystem::DeletePhysicsSystem()
{
#if WITH_JOLT_PHYSICS
	PhysicsSystem.Reset();
	StateRecorderFilter.Reset();
	ContactListener.Reset();
	BodyActivationListener.Reset();
	ObjectLayerPairFilter.Reset();
	ObjectVsBroadPhaseLayerFilter.Reset();
	BroadPhaseLayerInterface.Reset();
	JobSystem.Reset();
	TempAllocator.Reset();

	if (JPH::Factory::sInstance != nullptr)
	{
		JPH::UnregisterTypes();
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}
#endif // WITH_JOLT_PHYSICS
}

TArray<FJPRContactEvent> UJPRPhysicsSubsystem::ConsumeContactEvents()
{
#if WITH_JOLT_PHYSICS
	return ContactListener.IsValid() ? ContactListener->ConsumeEvents() : TArray<FJPRContactEvent>();
#else // WITH_JOLT_PHYSICS
	return {};
#endif
}

int32 UJPRPhysicsSubsystem::GetNumPendingContactEvents() const
{
#if WITH_JOLT_PHYSICS
	return ContactListener.IsValid() ? ContactListener->GetNumEvents() : 0;
#else // WITH_JOLT_PHYSICS
	return 0;
#endif
}

void UJPRPhysicsSubsystem::SetBodyObjectLayer(const uint32 BodyID, const uint16 Layer)
{
#if WITH_JOLT_PHYSICS
	GetBodyInterface().SetObjectLayer(JPH::BodyID(BodyID), static_cast<JPH::ObjectLayer>(Layer));
#endif // WITH_JOLT_PHYSICS
}

EJPRPhysicsMotionType UJPRPhysicsSubsystem::GetBodyMotionType(const uint32 BodyID) const
{
#if WITH_JOLT_PHYSICS
	return static_cast<EJPRPhysicsMotionType>(GetBodyInterface().GetMotionType(JPH::BodyID(BodyID)));
#else // WITH_JOLT_PHYSICS
	return EJPRPhysicsMotionType::Static;
#endif
}

void UJPRPhysicsSubsystem::SetBodyMotionType(const uint32 BodyID, const EJPRPhysicsMotionType MotionType, const EJPRPhysicsActivation ActivationMode)
{
#if WITH_JOLT_PHYSICS
	GetBodyInterface().SetMotionType(JPH::BodyID(BodyID), static_cast<JPH::EMotionType>(MotionType), static_cast<JPH::EActivation>(ActivationMode));
#endif // WITH_JOLT_PHYSICS
}

bool UJPRPhysicsSubsystem::CreateFixedConstraint(const uint32 BodyID1, const uint32 BodyID2, const bool bActivate)
{
#if WITH_JOLT_PHYSICS
	JPH::FixedConstraintSettings ConstraintSettings;
	ConstraintSettings.mAutoDetectPoint = true;

	const JPH::BodyID ConstraintBodies[] = { JPH::BodyID(BodyID1), JPH::BodyID(BodyID2) };
	JPH::BodyLockMultiWrite Lock(GetPhysicsSystem().GetBodyLockInterface(), ConstraintBodies, UE_ARRAY_COUNT(ConstraintBodies));
	JPH::Body* Body1 = Lock.GetBody(0);
	JPH::Body* Body2 = Lock.GetBody(1);
	if (!ensure(Body1 != nullptr) || !ensure(Body2 != nullptr))
	{
		return false;
	}

	JPH::TwoBodyConstraint* Constraint = ConstraintSettings.Create(*Body1, *Body2);
	if (Constraint == nullptr)
	{
		return false;
	}

	GetPhysicsSystem().AddConstraint(Constraint);
	if (bActivate)
	{
		GetBodyInterface().ActivateConstraint(Constraint);
	}
	return true;
#else // WITH_JOLT_PHYSICS
	return false;
#endif
}

void UJPRPhysicsSubsystem::RemoveFixedConstraints(const uint32 BodyID1, const uint32 BodyID2)
{
#if WITH_JOLT_PHYSICS
	const JPH::BodyID ConstraintBodies[] = { JPH::BodyID(BodyID1), JPH::BodyID(BodyID2) };
	const JPH::Constraints Constraints = GetPhysicsSystem().GetConstraints();
	for (JPH::Constraint* Constraint : Constraints)
	{
		const JPH::TwoBodyConstraint* TwoBodyConstraint = static_cast<JPH::TwoBodyConstraint*>(Constraint);
		const JPH::Body* Body1 = TwoBodyConstraint->GetBody1();
		const JPH::Body* Body2 = TwoBodyConstraint->GetBody2();
		if (ensure(Body1 != nullptr && Body2 != nullptr)
			&& (Body1->GetID() == ConstraintBodies[0] || Body1->GetID() == ConstraintBodies[1])
			&& (Body2->GetID() == ConstraintBodies[0] || Body2->GetID() == ConstraintBodies[1]))
		{
			GetPhysicsSystem().RemoveConstraint(Constraint);
		}
	}
#endif // WITH_JOLT_PHYSICS
}

void UJPRPhysicsSubsystem::CreatePhysicsSystem()
{
#if WITH_JOLT_PHYSICS
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::RegisterDefaultAllocator();
	if (JPH::Factory::sInstance == nullptr)
	{
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();
	}

	const UJPRPhysicsSettings* PhysicsSettings = GetDefault<UJPRPhysicsSettings>();
	TempAllocator = MakeShareable<JPH::TempAllocator>(new JPH::TempAllocatorImpl(10 * 1024 * 1024));
	JobSystem = MakeShared<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, PhysicsSettings->MaxNumThreads);
	BroadPhaseLayerInterface = JPR::Private::CreateBroadPhaseLayerInterface(*PhysicsLayer);
	ObjectVsBroadPhaseLayerFilter = JPR::Private::CreateObjectVsBroadPhaseLayerFilter(*PhysicsLayer);
	ObjectLayerPairFilter = JPR::Private::CreateObjectLayerPairFilter(*PhysicsLayer);
	StateRecorderFilter = MakeShared<FJPRStateRecorderFilter>([this](const uint32 BodyID) { return ShouldRestorePhysicsBody(BodyID); });
	BodyActivationListener = MakeShared<FJPRBodyActivationListener>();
	ContactListener = MakeShared<FJPRContactListener>();

	PhysicsSystem = MakeShared<JPH::PhysicsSystem>();
	PhysicsSystem->Init(PhysicsSettings->MaxBodies, PhysicsSettings->NumBodyMutexes, PhysicsSettings->MaxBodyPairs,
		PhysicsSettings->MaxContactConstraints, *BroadPhaseLayerInterface, *ObjectVsBroadPhaseLayerFilter, *ObjectLayerPairFilter);
	PhysicsSystem->SetBodyActivationListener(BodyActivationListener.Get());
	PhysicsSystem->SetContactListener(ContactListener.Get());
#endif // WITH_JOLT_PHYSICS
}

#if WITH_JOLT_PHYSICS
JPH::PhysicsSystem& UJPRPhysicsSubsystem::GetPhysicsSystem() const
{
	check(PhysicsSystem.IsValid());
	return *PhysicsSystem;
}

JPH::BodyInterface& UJPRPhysicsSubsystem::GetBodyInterface() const
{
	return GetPhysicsSystem().GetBodyInterface();
}
#endif // WITH_JOLT_PHYSICS

/*
void URecallPhysicsSubsystem::RunSample()
{
#if WITH_JOLT_PHYSICS
	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
	BodyInterface& body_interface = GetBodyInterface();

	// Next we can create a rigid body to serve as the floor, we make a large box
	// Create the settings for the collision volume (the shape). 
	// Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
	BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));

	// Create the shape
	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

	// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
	BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, (ObjectLayer)0);

	// Create the actual rigid body
	Body* floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

	// Add it to the world
	body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

	// Now create a dynamic body to bounce on the floor
	// Note that this uses the shorthand version of creating and adding a body to the world
	BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Dynamic, (ObjectLayer)1);
	BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
	// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
	body_interface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	PhysicsSystem->OptimizeBroadPhase();

	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	const float cDeltaTime = 1.0f / 60.0f;

	// Now we're ready to simulate the body, keep simulating until it goes to sleep
	uint step = 0;
	while (body_interface.IsActive(sphere_id))
	{
		// Next step
		++step;

		// Output current position and velocity of the sphere
		RVec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
		Vec3 velocity = body_interface.GetLinearVelocity(sphere_id);
		UE_LOG(LogRecallPhysics, Log, TEXT("Step %d: Position = (%.2f, %.2f, %.2f), Velocity = (%.2f, %.2f, %.2f)"),
			step, position.GetX(), position.GetY(), position.GetZ(), velocity.GetX(), velocity.GetY(), velocity.GetZ());

		// Step the world
		PhysicsSystem->Update(cDeltaTime, CollisionSteps, IntegrationSubSteps, TempAllocator.Get(), JobSystem.Get());
	}

	// Remove the sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
	body_interface.RemoveBody(sphere_id);

	// Destroy the sphere. After this the sphere ID is no longer valid.
	body_interface.DestroyBody(sphere_id);

	// Remove and destroy the floor
	body_interface.RemoveBody(floor->GetID());
	body_interface.DestroyBody(floor->GetID());
#endif // WITH_JOLT_PHYSICS
}
*/
