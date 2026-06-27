// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

constexpr double JoltPhysicsToUnrealUnitScale = 100.0;
constexpr double UnrealToJoltPhysicsUnitScale = 0.01;

inline FQuat UnrealToJoltPhysics(const FQuat& Q) { return FQuat(Q.X, Q.Z, -Q.Y, Q.W); }
inline FQuat JoltPhysicsToUnreal(const FQuat& Q) { return FQuat(Q.X, -Q.Z, Q.Y, Q.W); }

inline FVector UnrealToJoltPhysics(const FVector& V)
{
	return FVector(V.X * UnrealToJoltPhysicsUnitScale, V.Z * UnrealToJoltPhysicsUnitScale, -V.Y * UnrealToJoltPhysicsUnitScale);
}

inline FVector UnrealToJoltPhysics(const FVector3f& V)
{
	return FVector(V.X * UnrealToJoltPhysicsUnitScale, V.Z * UnrealToJoltPhysicsUnitScale, -V.Y * UnrealToJoltPhysicsUnitScale);
}

inline FVector JoltPhysicsToUnreal(const FVector& V)
{
	return FVector(V.X * JoltPhysicsToUnrealUnitScale, -V.Z * JoltPhysicsToUnrealUnitScale, V.Y * JoltPhysicsToUnrealUnitScale);
}

inline FVector UnrealToJoltPhysicsDirection(const FVector& V) { return FVector(V.X, V.Z, -V.Y); }
inline FVector JoltPhysicsToUnrealDirection(const FVector& V) { return FVector(V.X, -V.Z, V.Y); }

inline FVector UnrealToJoltPhysicsScale(const FVector& V)
{
	return FVector(V.X * UnrealToJoltPhysicsUnitScale, V.Z * UnrealToJoltPhysicsUnitScale, V.Y * UnrealToJoltPhysicsUnitScale);
}

inline FVector JoltPhysicsToUnrealScale(const FVector& V)
{
	return FVector(V.X * JoltPhysicsToUnrealUnitScale, V.Z * JoltPhysicsToUnrealUnitScale, V.Y * JoltPhysicsToUnrealUnitScale);
}
