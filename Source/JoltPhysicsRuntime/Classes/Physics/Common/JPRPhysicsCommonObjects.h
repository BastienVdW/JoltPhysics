// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Physics/JPRPhysicsBody.h"

struct FJPRPhysicsCapsuleShape;

/**
* Wrapper Object for JPH::CapsuleShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsCapsuleBody : public FJPRPhysicsBody
{
public:
	void InitCapsule(float Radius, float HalfHeight, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);
	void InitCapsule(const FJPRPhysicsCapsuleShape& CapsuleShape, const FJPRPhysicsBodyParameters& Params,
		uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

};

/**
* Wrapper Object for JPH::SphereShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsSphereBody : public FJPRPhysicsBody
{
public:
	void InitSphere(float Radius, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

};

/**
* Wrapper Object for JPH::BoxShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsBoxBody : public FJPRPhysicsBody
{
public:
	void InitBox(const FVector& Extents, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

};

/**
* Wrapper Object for JPH::MeshShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsMeshBody : public FJPRPhysicsBody
{
public:
	void InitMesh(const struct FJPRPhysicsMeshShape& MeshShape, const FJPRPhysicsBodyParameters& Params,
		uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
private:
	TArray<FVector> world_vertices;
	TArray<int32> world_triangles;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
};

class JOLTPHYSICSRUNTIME_API FJPRPhysicsStaticCompoundBody : public FJPRPhysicsBody
{
public:
	void InitStaticCompound(const struct FJPRPhysicsStaticCompoundShape& StaticCompoundShape, const FJPRPhysicsBodyParameters& Params,
		uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
private:
	struct JPRPhysicsStaticCompoundMesh
	{
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		FVector Position = FVector::ZeroVector;
		FQuat Rotation = FQuat::Identity;
		FBox BoundingBox;
	};
	TArray<JPRPhysicsStaticCompoundMesh> Meshes;
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
};

/**
* Wrapper Object for JPH::ConvexHullShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsConvexHullBody : public FJPRPhysicsBody
{
public:
	void InitConvexHull(const TArray<FVector3f>& WorldVertices, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

};

/**
* Wrapper Object for JPH::HeightFieldShape.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsHeightFieldBody : public FJPRPhysicsBody
{
public:
	void InitHeightField(int32 SizeX, int32 SizeY, const TArray<float>& Heights, const FVector& Scale,
		const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);

public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

};
