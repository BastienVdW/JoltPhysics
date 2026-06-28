// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Physics/JPRPhysicsTypes.h"

#include "JPRPhysicsSubsystem.generated.h"

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class BodyInterface;
	class BroadPhaseLayerInterface;
	class JobSystemThreadPool;
	class ObjectLayerPairFilter;
	class ObjectVsBroadPhaseLayerFilter;
	class PhysicsSystem;
	class StateRecorderFilter;
	class TempAllocator;
}
#endif // WITH_JOLT_PHYSICS

class FJPRPhysicsBody;

struct JOLTPHYSICSRUNTIME_API FJPRContactEvent
{
	uint32 BodyID1 = 0;
	uint32 BodyID2 = 0;
	FVector Velocity1 = FVector::ZeroVector;
	FVector Velocity2 = FVector::ZeroVector;
	bool bIsStatic1 = false;
	bool bIsStatic2 = false;
	bool bIsSensor1 = false;
	bool bIsSensor2 = false;
	FVector ImpactPoint1 = FVector::ZeroVector;
	FVector ImpactPoint2 = FVector::ZeroVector;
	FVector ImpactNormal1 = FVector::ZeroVector;
	FVector ImpactNormal2 = FVector::ZeroVector;
	uint32 SubShapeID1 = 0;
	uint32 SubShapeID2 = 0;
};

/**
 * Unreal-facing owner for a Jolt physics world.
 *
 * World creation, ticking, contacts, and state serialization will move into this base incrementally.
 * Recall derives from this type so existing subsystem references remain stable throughout the migration.
 */
UCLASS(Abstract)
class JOLTPHYSICSRUNTIME_API UJPRPhysicsSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	void SetBodyObjectLayer(uint32 BodyID, uint16 Layer);
	
	EJPRPhysicsMotionType GetBodyMotionType(uint32 BodyID) const;
	void SetBodyMotionType(uint32 BodyID, EJPRPhysicsMotionType MotionType, EJPRPhysicsActivation ActivationMode);
	
	bool CreateFixedConstraint(uint32 BodyID1, uint32 BodyID2, bool bActivate);
	void RemoveFixedConstraints(uint32 BodyID1, uint32 BodyID2);

	TArray<FJPRContactEvent> ConsumeContactEvents();
	int32 GetNumPendingContactEvents() const;
	
#if WITH_JOLT_PHYSICS
	JPH::PhysicsSystem& GetPhysicsSystem() const;
	JPH::BodyInterface& GetBodyInterface() const;
	TSharedPtr<JPH::PhysicsSystem> GetPhysicsSystemPtr() const { return PhysicsSystem; }
	TSharedPtr<JPH::TempAllocator> GetTempAllocator() const { return TempAllocator; }
#endif // WITH_JOLT_PHYSICS
	
	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// USubsystem implementation End
	
	// FTickableGameObject implementation Begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }
	// FTickableGameObject implementation End
	
protected:
	void StepPhysics(float DeltaTime, int32 CollisionSteps, float TimeDilation = 1.0f);
	bool HasPhysicsSystem() const;
	
	TArray<uint8> SavePhysicsState(const TArray<TSharedPtr<FJPRPhysicsBody>>& Bodies) const;
	bool RestorePhysicsState(const TArray<uint8>& StateData, const TArray<TSharedPtr<FJPRPhysicsBody>>& Bodies) const;

	void CreatePhysicsSystem();
	void DeletePhysicsSystem();
	
	virtual bool ShouldRestorePhysicsBody(const uint32 BodyID) const { return true; }

	UPROPERTY(Transient)
	TObjectPtr<const class UJPRPhysicsLayerDataAsset> PhysicsLayer;
	
#if WITH_JOLT_PHYSICS
	TSharedPtr<JPH::TempAllocator> TempAllocator;
	TSharedPtr<JPH::JobSystemThreadPool> JobSystem;
	TSharedPtr<JPH::PhysicsSystem> PhysicsSystem;
	TSharedPtr<JPH::BroadPhaseLayerInterface> BroadPhaseLayerInterface;
	TSharedPtr<JPH::ObjectVsBroadPhaseLayerFilter> ObjectVsBroadPhaseLayerFilter;
	TSharedPtr<JPH::ObjectLayerPairFilter> ObjectLayerPairFilter;
	TSharedPtr<JPH::StateRecorderFilter> StateRecorderFilter;
	TSharedPtr<class FJPRBodyActivationListener> BodyActivationListener;
	TSharedPtr<class FJPRContactListener> ContactListener;
#endif // WITH_JOLT_PHYSICS
	
	// void RunSample();

};
