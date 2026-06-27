// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");

#pragma once

#include "CoreMinimal.h"

#ifndef WITH_JOLT_PHYSICS
#define WITH_JOLT_PHYSICS 0
#endif

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class PhysicsSystem;
}
#endif

class JOLTPHYSICSRUNTIME_API FJPRPhysicsGroundCollisionTesterRay
{
public:
	FJPRPhysicsGroundCollisionTesterRay(int32 InObjectLayer, FVector InUp = FVector::UpVector, float InMaxSlopeAngle = 80.0f);

#if WITH_JOLT_PHYSICS
	bool Collide(JPH::PhysicsSystem& InPhysicsSystem, uint32 InBodyID, const FVector& InRayOrigin, const FVector& InRayDirection, float InRayLength,
		uint32& OutContactBodyID, FVector& OutContactPosition, FVector& OutContactNormal) const;
#endif

private:
	int32 ObjectLayer;
	FVector Up;
	float CosMaxSlopeAngle;
};
