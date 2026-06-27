// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0

#include "System/JPRPhysicsSubsystem.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "Physics/JPRPhysicsLayerFilters.h"
#include "Settings/JPRPhysicsSettings.h"

#if WITH_JOLT_PHYSICS
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/PhysicsSystem.h>
#endif

void UJPRPhysicsSubsystem::StepPhysics(const float DeltaTime, const int32 CollisionSteps, const float TimeDilation)
{
#if WITH_JOLT_PHYSICS
	check(PhysicsSystem.IsValid());
	check(TempAllocator.IsValid());
	check(JobSystem.IsValid());
	PhysicsSystem->Update(DeltaTime * TimeDilation, CollisionSteps, TempAllocator.Get(), JobSystem.Get());
	JobSystem->WaitThreads();
#endif
}

void UJPRPhysicsSubsystem::DeletePhysicsSystem()
{
#if WITH_JOLT_PHYSICS
	PhysicsSystem.Reset();
	StateRecorderFilter.Reset();
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
#endif
}

#if WITH_JOLT_PHYSICS
void UJPRPhysicsSubsystem::CreatePhysicsSystem(const UJPRPhysicsLayerDataAsset& PhysicsLayer, JPH::BodyActivationListener* BodyActivationListener,
	JPH::ContactListener* ContactListener, const TSharedPtr<JPH::StateRecorderFilter>& InStateRecorderFilter)
{
	JPH::RegisterDefaultAllocator();
	if (JPH::Factory::sInstance == nullptr)
	{
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();
	}

	const UJPRPhysicsSettings* PhysicsSettings = GetDefault<UJPRPhysicsSettings>();
	TempAllocator = MakeShareable<JPH::TempAllocator>(new JPH::TempAllocatorImpl(10 * 1024 * 1024));
	JobSystem = MakeShared<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, PhysicsSettings->MaxNumThreads);
	BroadPhaseLayerInterface = JPR::Private::CreateBroadPhaseLayerInterface(PhysicsLayer);
	ObjectVsBroadPhaseLayerFilter = JPR::Private::CreateObjectVsBroadPhaseLayerFilter(PhysicsLayer);
	ObjectLayerPairFilter = JPR::Private::CreateObjectLayerPairFilter(PhysicsLayer);
	StateRecorderFilter = InStateRecorderFilter;

	PhysicsSystem = MakeShared<JPH::PhysicsSystem>();
	PhysicsSystem->Init(PhysicsSettings->MaxBodies, PhysicsSettings->NumBodyMutexes, PhysicsSettings->MaxBodyPairs,
		PhysicsSettings->MaxContactConstraints, *BroadPhaseLayerInterface, *ObjectVsBroadPhaseLayerFilter, *ObjectLayerPairFilter);
	PhysicsSystem->SetBodyActivationListener(BodyActivationListener);
	PhysicsSystem->SetContactListener(ContactListener);
}
#endif

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
#endif
