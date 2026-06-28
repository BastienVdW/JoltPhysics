// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Physics/JPRPhysicsShapeTypes.h"

#include "JPRPhysicsCommonShapeTypes.generated.h"

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsBoxShape : public FJPRPhysicsShape
{
	GENERATED_BODY()

	FJPRPhysicsBoxShape();
	FJPRPhysicsBoxShape(const FVector& InExtents);

public:
	UPROPERTY(VisibleAnywhere)
	FVector WorldExtents = static_cast<FVector>(50.0);
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsSphereShape : public FJPRPhysicsShape
{
	GENERATED_BODY()
	
	FJPRPhysicsSphereShape();
	FJPRPhysicsSphereShape(float Radius);

public:
	UPROPERTY(VisibleAnywhere)
	float RadiusCentimeters = 100.0f;

	FORCEINLINE FVector GetSphereUnrealExtents() const { return FVector(RadiusCentimeters, RadiusCentimeters, RadiusCentimeters); }
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsCapsuleShape : public FJPRPhysicsShape
{
	GENERATED_BODY()
	
	FJPRPhysicsCapsuleShape();
	FJPRPhysicsCapsuleShape(float Radius, float HalfHeight);

public:
	UPROPERTY(VisibleAnywhere)
	float RadiusCentimeters{ 34.0f };

	UPROPERTY(VisibleAnywhere, DisplayName="HalfHeightOfCylinder")
	float HalfHeightCentimeters{ 88.0f };
};

USTRUCT(BlueprintType)
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsMeshShapeSettings
{
	GENERATED_BODY()
	
	/**
	 * Maximum number of triangles in each leaf of the axis aligned box tree. This is a balance between memory and performance. Can be in the range [1, MeshShape::MaxTrianglesPerLeaf].
	 * Sensible values are between 4 (for better performance) and 8 (for less memory usage)
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin=1, ClampMax=8))
	int32 MaxTrianglesPerLeaf = 8;

	/**
	 * Threshold angle (if the angle between the two triangles is bigger than this, the edge is active, note that a concave edge is always inactive).
	 * Setting this value too small can cause ghost collisions with edges, setting it too big can cause depenetration artifacts (objects not depenetrating quickly).
	 * Valid ranges are between 0 degrees and 90 degrees. The default value is 5 degrees.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Degrees, ClampMin=0.0, ClampMax=90.0))
	float ActiveEdgeCosThresholdAngle = 5.0f;

	/**
	 * When true, we store the user data coming from Triangle::mUserData or IndexedTriangle::mUserData in the mesh shape.
	 * This can be used to store additional data like the original index of the triangle in the mesh.
	 * Can be retrieved using MeshShape::GetTriangleUserData.
	 * Turning this on increases the memory used by the MeshShape by roughly 25%.
	 */
	UPROPERTY(EditAnywhere)
	bool PerTriangleUserData = false;
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsMeshShape : public FJPRPhysicsShape
{
	GENERATED_BODY()
	
	FJPRPhysicsMeshShape();
	FJPRPhysicsMeshShape(const TArray<FVector3f>& InVertices, const TArray<int32>& InTriangles);

public:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector3f> Vertices;

	UPROPERTY(VisibleAnywhere)
	TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere)
	FJPRPhysicsMeshShapeSettings MeshShapeSettings;
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsConvexHullShape : public FJPRPhysicsShape
{
	GENERATED_BODY()

	FJPRPhysicsConvexHullShape();
	FJPRPhysicsConvexHullShape(const TArray<FVector3f>& InVertices);

public:
	UPROPERTY(VisibleAnywhere)
	TArray<FVector3f> Vertices;
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsHeightFieldShape : public FJPRPhysicsShape
{
	GENERATED_BODY()
	
	FJPRPhysicsHeightFieldShape();
	FJPRPhysicsHeightFieldShape(int32 InSizeX, int32 InSizeY, const TArray<float>& InHeights, const FVector& InScale);

public:
	UPROPERTY(VisibleAnywhere)
	int32 SizeX = 0;
	
	UPROPERTY(VisibleAnywhere)
	int32 SizeY = 0;
	
	UPROPERTY(VisibleAnywhere)
	TArray<float> Heights;
	
	UPROPERTY(VisibleAnywhere)
	FVector Scale = FVector::OneVector;

	FORCEINLINE bool IsValid() const { return Heights.Num() && SizeX > 0 && SizeY > 0 && Scale.SquaredLength() > 0.0f; }
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsStaticCompoundSubShape : public FJPRPhysicsShape
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FJPRPhysicsMeshShape MeshShape;
	
	UPROPERTY(VisibleAnywhere)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere)
	FQuat Rotation = FQuat::Identity;	
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsStaticCompoundShape : public FJPRPhysicsShape
{
	GENERATED_BODY()

	FJPRPhysicsStaticCompoundShape();
	
	UPROPERTY(VisibleAnywhere)
	TArray<FJPRPhysicsStaticCompoundSubShape> SubShapes;
};
