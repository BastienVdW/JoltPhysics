// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "JPRPhysicsCommonShapeTypes.h"

#include "Physics/Common/JPRPhysicsCommonObjectFactory.h"

FJPRPhysicsBoxShape::FJPRPhysicsBoxShape()
	: Super(UJPRPhysicsBoxObjectFactory::StaticClass())
{
}

FJPRPhysicsBoxShape::FJPRPhysicsBoxShape(const FVector& InExtents)
	: Super(UJPRPhysicsBoxObjectFactory::StaticClass())
	, WorldExtents(InExtents)
{
}

FJPRPhysicsSphereShape::FJPRPhysicsSphereShape()
	: Super(UJPRPhysicsSphereObjectFactory::StaticClass())
{
}

FJPRPhysicsSphereShape::FJPRPhysicsSphereShape(float Radius)
	: Super(UJPRPhysicsSphereObjectFactory::StaticClass())
	, RadiusCentimeters(Radius)
{
}

FJPRPhysicsCapsuleShape::FJPRPhysicsCapsuleShape()
	: Super(UJPRPhysicsCapsuleObjectFactory::StaticClass())
{
}

FJPRPhysicsCapsuleShape::FJPRPhysicsCapsuleShape(float Radius, float HalfHeight)
	: Super(UJPRPhysicsCapsuleObjectFactory::StaticClass())
	, RadiusCentimeters(Radius)
	, HalfHeightCentimeters(HalfHeight)
{
}

FJPRPhysicsMeshShape::FJPRPhysicsMeshShape()
	: Super(UJPRPhysicsMeshObjectFactory::StaticClass())
{
}

FJPRPhysicsMeshShape::FJPRPhysicsMeshShape(const TArray<FVector3f>& InVertices, const TArray<int32>& InTriangles)
	: Super(UJPRPhysicsMeshObjectFactory::StaticClass())
	, Vertices(InVertices)
	, Triangles(InTriangles)
{
}

FJPRPhysicsConvexHullShape::FJPRPhysicsConvexHullShape()
	: Super(UJPRPhysicsConvexHullObjectFactory::StaticClass())
{
}

FJPRPhysicsConvexHullShape::FJPRPhysicsConvexHullShape(const TArray<FVector3f>& InVertices)
	: Super(UJPRPhysicsConvexHullObjectFactory::StaticClass())
	, Vertices(InVertices)
{
}

FJPRPhysicsHeightFieldShape::FJPRPhysicsHeightFieldShape()
	: Super(UJPRPhysicsHeightFieldObjectFactory::StaticClass())
{
}

FJPRPhysicsHeightFieldShape::FJPRPhysicsHeightFieldShape(int32 InSizeX, int32 InSizeY,
	const TArray<float>& InHeights, const FVector& InScale)
	: Super(UJPRPhysicsHeightFieldObjectFactory::StaticClass())
	, SizeX(InSizeX)
	, SizeY(InSizeY)
	, Heights(InHeights)
	, Scale(InScale)
{
}

FJPRPhysicsStaticCompoundShape::FJPRPhysicsStaticCompoundShape()
	: Super(UJPRPhysicsStaticCompoundObjectFactory::StaticClass())
{
}
