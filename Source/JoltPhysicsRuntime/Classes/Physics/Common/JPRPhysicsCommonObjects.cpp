// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "JPRPhysicsCommonObjects.h"

#include "Algo/AnyOf.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "JPRPhysicsCommonShapeTypes.h"
#include "Physics/JPRPhysicsTypes.h"
#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

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

// FJPRPhysicsSphereBody Begin
void FJPRPhysicsSphereBody::InitSphere(float Radius, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	BodyCreationSettings sphere_settings(new SphereShape(Radius * UnrealToJoltPhysicsUnitScale), RVec3::sZero(), Quat::sIdentity(), (EMotionType)Params.MotionType, (ObjectLayer)Layer);
	
	SetupBodyCreationSettings(sphere_settings, Params);

	CreateAndSetBody(sphere_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsSphereBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const ShapeRefC ShapeRefC = GetBodyInterface().GetShape(*body_id.Get());
		const SphereShape* Shape = static_cast<const SphereShape*>(ShapeRefC.GetPtr());

		const float Radius = Shape->GetRadius() * JoltPhysicsToUnrealUnitScale;

		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		DrawDebugSphere(World, WorldPosition, Radius, 12, Color);

		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			const FVector WorldVel = GetLinearVelocityPerSecond();
			DrawDebugString(World, WorldPosition, FString::Printf(TEXT("Vel: %s"), *WorldVel.ToString()));
		}
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsSphereBody End

// FJPRPhysicsBoxBody Begin
void FJPRPhysicsBoxBody::InitBox(const FVector& Extents, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	const FVector PhysicsExtents = UnrealToJoltPhysicsScale(Extents);
	const Vec3 HalfExtents(PhysicsExtents.X, PhysicsExtents.Y, PhysicsExtents.Z);

	BodyCreationSettings box_settings(new BoxShape(HalfExtents), RVec3::sZero(), Quat::sIdentity(), (EMotionType)Params.MotionType, (ObjectLayer)Layer);

	SetupBodyCreationSettings(box_settings, Params);

	CreateAndSetBody(box_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsBoxBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const ShapeRefC ShapeRefC = GetBodyInterface().GetShape(*body_id.Get());
		const BoxShape* Shape = static_cast<const BoxShape*>(ShapeRefC.GetPtr());

		const Vec3 HalfExtents = Shape->GetHalfExtent();
		const FVector WorldExtents = JoltPhysicsToUnrealScale(FVector(HalfExtents.GetX(), HalfExtents.GetY(), HalfExtents.GetZ()));

		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		DrawDebugBox(World, WorldPosition, WorldExtents, WorldRotation, Color);

		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			const FVector WorldVel = GetLinearVelocityPerSecond();
			DrawDebugString(World, WorldPosition, FString::Printf(TEXT("Vel: %s"), *WorldVel.ToString()));
		}
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsBoxBody End

// FJPRPhysicsCapsuleBody Begin
void FJPRPhysicsCapsuleBody::InitCapsule(float Radius, float HalfHeight, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	BodyCreationSettings capsule_settings(new CapsuleShape(HalfHeight * UnrealToJoltPhysicsUnitScale, Radius * UnrealToJoltPhysicsUnitScale), RVec3::sZero(), Quat::sIdentity(), (EMotionType)Params.MotionType, (ObjectLayer)Layer);

	SetupBodyCreationSettings(capsule_settings, Params);

	CreateAndSetBody(capsule_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsCapsuleBody::InitCapsule(const FJPRPhysicsCapsuleShape& CapsuleShape,
	const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
	InitCapsule(CapsuleShape.RadiusCentimeters, CapsuleShape.HalfHeightCentimeters, Params, InBodyID, Layer);
}

void FJPRPhysicsCapsuleBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		const ShapeRefC ShapeRefC = GetBodyInterface().GetShape(*body_id.Get());
		const JPH::CapsuleShape* Shape = static_cast<const JPH::CapsuleShape*>(ShapeRefC.GetPtr());

		const float HalfHeight = (Shape->GetHalfHeightOfCylinder() + Shape->GetRadius()) * JoltPhysicsToUnrealUnitScale;
		const float Radius = Shape->GetRadius() * JoltPhysicsToUnrealUnitScale;

		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		if (!WorldRotation.IsNormalized())
		{
			return;
		}

		DrawDebugCapsule(World, WorldPosition, HalfHeight, Radius, WorldRotation, Color);

		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			const FVector WorldVel = GetLinearVelocityPerSecond();
			DrawDebugString(World, WorldPosition, FString::Printf(TEXT("Vel: %s"), *WorldVel.ToString()));
		}
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsCapsuleBody End

// FJPRPhysicsMeshBody Begin
#if WITH_JOLT_PHYSICS
static MeshShapeSettings* CreateMeshShapeSettings(const FJPRPhysicsMeshShape& MeshShape,
	TArray<FVector>& OutVertices, TArray<int32>& OutTriangles)
{
	OutTriangles = MeshShape.Triangles;

	VertexList vertices;

	OutVertices.SetNumUninitialized(MeshShape.Vertices.Num());

	for (int32 VertexIndex = 0; VertexIndex < MeshShape.Vertices.Num(); VertexIndex++)
	{
		OutVertices[VertexIndex] = static_cast<FVector>(MeshShape.Vertices[VertexIndex]);

		const FVector PhysicsVertex = UnrealToJoltPhysics(OutVertices[VertexIndex]);
		vertices.push_back(Float3(PhysicsVertex.X, PhysicsVertex.Y, PhysicsVertex.Z));
	}

	IndexedTriangleList triangles;

	for (int32 TriangleIndex = 0; TriangleIndex < OutTriangles.Num() - 2; TriangleIndex += 3)
	{
		// Flip triangles since they seem to be reversed in Jolt.
		triangles.push_back(IndexedTriangle(OutTriangles[TriangleIndex + 2], OutTriangles[TriangleIndex + 1], OutTriangles[TriangleIndex]));
	}

	const float ActiveEdgeCosThresholdAngleRad = FMath::DegreesToRadians(MeshShape.MeshShapeSettings.ActiveEdgeCosThresholdAngle);

	MeshShapeSettings* mesh_settings = new MeshShapeSettings(vertices, triangles);
	mesh_settings->mMaxTrianglesPerLeaf = MeshShape.MeshShapeSettings.MaxTrianglesPerLeaf;
	mesh_settings->mActiveEdgeCosThresholdAngle = FMath::Cos(ActiveEdgeCosThresholdAngleRad);
	mesh_settings->mPerTriangleUserData = MeshShape.MeshShapeSettings.PerTriangleUserData;

	return mesh_settings;
}
#endif // WITH_JOLT_PHYSICS

void FJPRPhysicsMeshBody::InitMesh(const FJPRPhysicsMeshShape& MeshShape, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	MeshShapeSettings* mesh_settings = CreateMeshShapeSettings(MeshShape, world_vertices, world_triangles);
#else // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	MeshShapeSettings* mesh_settings = CreateMeshShapeSettings(MeshShape, Vertices, Triangles);
#endif

	BodyCreationSettings body_creation_settings(mesh_settings, RVec3::sZero(), Quat::sIdentity(),
		static_cast<EMotionType>(Params.MotionType), static_cast<ObjectLayer>(Layer));

	SetupBodyCreationSettings(body_creation_settings, Params);
	
	if (Params.MotionType != EJPRPhysicsMotionType::Static || Params.bAllowDynamicOrKinematic)
	{
		body_creation_settings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
		body_creation_settings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3::sReplicate(1.0f), 1000.0f);
	}

	CreateAndSetBody(body_creation_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

static void DrawDebugMeshShape(const UWorld* World,
	const TArray<FVector>& MeshVertices, const TArray<int32>& MeshTriangles, const FColor& Color, float Thickness = 3.0f)
{
	DrawDebugMesh(World, MeshVertices, MeshTriangles, Color, false, -1.0f, 0);

	for (int32 TriangleIndex = 0; TriangleIndex < MeshTriangles.Num() - 2; TriangleIndex += 3)
	{
		const FVector& v1 = MeshVertices[MeshTriangles[TriangleIndex + 2]];
		const FVector& v2 = MeshVertices[MeshTriangles[TriangleIndex + 1]];
		const FVector& v3 = MeshVertices[MeshTriangles[TriangleIndex]];

		const FVector center = (v1 + v2 + v3) / 3.0;

		FVector normal = (v2 - v1).Cross(v3 - v1);
		normal.Normalize();

		DrawDebugDirectionalArrow(World, center, center + normal * 30.0f, 10.0f, FColor::Black,
			false, -1.0f, 0, Thickness);
	}
}

void FJPRPhysicsMeshBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		const FTransform WorldTransform(WorldRotation, WorldPosition, static_cast<FVector>(1.0));

		TArray<FVector> WorldVertices;
		WorldVertices.SetNumUninitialized(world_vertices.Num());

		ParallelFor(world_vertices.Num(), [this, &WorldTransform, &WorldVertices](int32 VertexIndex)
		{
			WorldVertices[VertexIndex] = WorldTransform.TransformPosition(world_vertices[VertexIndex]);
		});

		DrawDebugMeshShape(World, WorldVertices, world_triangles, Color);
		
		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			const FVector WorldVel = GetLinearVelocityPerSecond();
			DrawDebugString(World, WorldPosition, FString::Printf(TEXT("Vel: %s"), *WorldVel.ToString()));
		}		
	}
#endif // WITH_JOLT_PHYSICS
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
// FJPRPhysicsMeshBody End

// FJPRPhysicsStaticCompoundBody Begin
void FJPRPhysicsStaticCompoundBody::InitStaticCompound(
	const struct FJPRPhysicsStaticCompoundShape& StaticCompoundShape, const FJPRPhysicsBodyParameters& Params,
	uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	StaticCompoundShapeSettings* static_compound_shape_settings = new StaticCompoundShapeSettings();
	
	for (const FJPRPhysicsStaticCompoundSubShape& SubShape : StaticCompoundShape.SubShapes)
	{
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		MeshShapeSettings* mesh_settings = CreateMeshShapeSettings(SubShape.MeshShape, Vertices, Triangles);
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		{			
			JPRPhysicsStaticCompoundMesh& Mesh = Meshes.AddDefaulted_GetRef();
			Mesh.Position = SubShape.Position;
			Mesh.Rotation = SubShape.Rotation;
			Mesh.Triangles = Triangles;

			const FTransform WorldTransform(SubShape.Rotation, SubShape.Position, static_cast<FVector>(1.0));

			Mesh.Vertices.SetNumUninitialized(Vertices.Num());
		
			ParallelFor(Vertices.Num(), [&Mesh, &WorldTransform, &Vertices](int32 VertexIndex)
			{
				Mesh.Vertices[VertexIndex] = WorldTransform.TransformPosition(Vertices[VertexIndex]);
			});

			Mesh.BoundingBox = FBox(Mesh.Vertices);
		}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		
		const FVector PhysicsPos = UnrealToJoltPhysics(SubShape.Position);
		const RVec3 Pos(PhysicsPos.X, PhysicsPos.Y, PhysicsPos.Z);
		const FQuat Rot = UnrealToJoltPhysics(SubShape.Rotation);
		
		static_compound_shape_settings->AddShape(Pos, Quat(Rot.X, Rot.Y, Rot.Z, Rot.W), mesh_settings);
	}

	BodyCreationSettings body_creation_settings(static_compound_shape_settings, RVec3::sZero(), Quat::sIdentity(),
	static_cast<EMotionType>(Params.MotionType), static_cast<ObjectLayer>(Layer));

	SetupBodyCreationSettings(body_creation_settings, Params);
	
	if (Params.MotionType != EJPRPhysicsMotionType::Static || Params.bAllowDynamicOrKinematic)
	{
		body_creation_settings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
		body_creation_settings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3::sReplicate(1.0f), 1000.0f);
	}

	CreateAndSetBody(body_creation_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsStaticCompoundBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	TArray<FVector> ViewTargetPositions;

	for (int32 PlayerIndex = 0; PlayerIndex < UGameplayStatics::GetNumPlayerControllers(World); PlayerIndex++)
	{
		const APlayerController* PC = UGameplayStatics::GetPlayerController(World, PlayerIndex);
		const AActor* ViewTarget = IsValid(PC) ? PC->GetViewTarget() : nullptr;
		if (IsValid(ViewTarget))
		{
			ViewTargetPositions.Add(PC->GetViewTarget()->GetActorLocation());
		}
	}
	
	TArray<bool> NearVertices;
	TArray<int32> NearTriangles;
	
	for (int32 MeshIndex = 0; MeshIndex < Meshes.Num(); MeshIndex++)
	{
		constexpr double CutoffDistance = 10000.0; // 100 meters
		
		const JPRPhysicsStaticCompoundMesh& Mesh = Meshes[MeshIndex];		
		const bool bBoundingBoxNearAnyViewTarget = Algo::AnyOf(ViewTargetPositions, [&Mesh](const FVector& ViewTargetPosition)
		{
			return Mesh.BoundingBox.ComputeSquaredDistanceToPoint(ViewTargetPosition) < CutoffDistance * CutoffDistance;
		});
		if (!bBoundingBoxNearAnyViewTarget)
		{
			continue;
		}

		const TArray<FVector>& Vertices = Mesh.Vertices;

		NearVertices.SetNum(Vertices.Num(), EAllowShrinking::No);

		ParallelFor(Vertices.Num(), [&ViewTargetPositions, &Vertices, &NearVertices](int32 VertexIndex)
		{
			const FVector& WorldVertex = Vertices[VertexIndex];
			NearVertices[VertexIndex] = Algo::AnyOf(ViewTargetPositions, [&WorldVertex](const FVector& ViewTargetPosition)
			{
				return FVector::DistSquared(WorldVertex, ViewTargetPosition) < CutoffDistance * CutoffDistance;
			});
		});

		NearTriangles.Reset();
		
		for (int32 TriangleIndex = 0; TriangleIndex < Mesh.Triangles.Num(); TriangleIndex += 3)
		{
			bool bVisibleTriangle = true;
			
			for (int32 TriangleOffset = 0; TriangleOffset < 3; TriangleOffset++)
			{
				const int32 VertexIndex = Mesh.Triangles[TriangleIndex + TriangleOffset];
				if (!NearVertices[VertexIndex])
				{
					bVisibleTriangle = false;
					break;
				}
			}

			if (bVisibleTriangle)
			{
				NearTriangles.Append({
					Mesh.Triangles[TriangleIndex],
					Mesh.Triangles[TriangleIndex + 1],
					Mesh.Triangles[TriangleIndex + 2]
				});
			}
		}

		if (NearTriangles.Num() == 0)
		{
			continue;
		}
			
		static const TArray<FColor> MeshColors = { FColor::Green, FColor::Blue, FColor::Yellow };
		const FColor& MeshColor = MeshColors[MeshIndex % MeshColors.Num()];

		DrawDebugMeshShape(World, Mesh.Vertices, NearTriangles, MeshColor);
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}
// FJPRPhysicsStaticCompoundBody End

// FJPRPhysicsConvexHullBody Begin
void FJPRPhysicsConvexHullBody::InitConvexHull(const TArray<FVector3f>& WorldVertices, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	JPH::Array<Vec3> vertices;

	for (int32 VertexIndex = 0; VertexIndex < WorldVertices.Num(); VertexIndex++)
	{
		const FVector PhysicsVertex = UnrealToJoltPhysics(WorldVertices[VertexIndex]);
		vertices.push_back(Vec3(PhysicsVertex.X, PhysicsVertex.Y, PhysicsVertex.Z));
	}

	ConvexHullShapeSettings* mesh_settings = new ConvexHullShapeSettings(vertices);

	BodyCreationSettings body_creation_settings(mesh_settings, RVec3::sZero(), Quat::sIdentity(), (EMotionType)Params.MotionType, (ObjectLayer)Layer);

	SetupBodyCreationSettings(body_creation_settings, Params);
	
	CreateAndSetBody(body_creation_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsConvexHullBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		ShapeRefC Shape = GetBodyInterface().GetShape(*body_id.Get());
		const ConvexHullShape* ConvexHullShapePtr = static_cast<const ConvexHullShape*>(Shape.GetPtr());
		if (ConvexHullShapePtr == nullptr)
		{
			return;
		}

		const Quat Rotation = GetBodyInterface().GetRotation(*body_id.Get());
		const RVec3 PositionCOM = GetBodyInterface().GetCenterOfMassPosition(*body_id.Get());

		TArray<FVector> Vertices;
		TArray<int32> Triangles;

		Shape::GetTrianglesContext context;
#ifndef JPH_DOUBLE_PRECISION
		ConvexHullShapePtr->GetTrianglesStart(context, AABox(), PositionCOM, Rotation, Vec3Arg(1.f, 1.f, 1.f));
#else // JPH_DOUBLE_PRECISION
		ConvexHullShapePtr->GetTrianglesStart(context, AABox(), Position.ToVec3RoundDown(), Rotation, Vec3Arg(1.f, 1.f, 1.f));
#endif

		JPH::Array<Float3> vertices;
		vertices.resize(Shape::cGetTrianglesMinTrianglesRequested * 3);

		int32 triangle_count = 0;
		do
		{
			triangle_count = ConvexHullShapePtr->GetTrianglesNext(context, Shape::cGetTrianglesMinTrianglesRequested, &vertices[0]);

			Vertices.Reserve(Vertices.Num() + 3 * triangle_count);
			Triangles.Reserve(Triangles.Num() + triangle_count);

			for (int32 triangle_index = 0; triangle_index < triangle_count; triangle_index++)
			{
				const int32 vertex_start = triangle_index * 3;

				const Float3 p1 = vertices[vertex_start];
				const Float3 p2 = vertices[vertex_start + 1];
				const Float3 p3 = vertices[vertex_start + 2];

				Vertices.Add(JoltPhysicsToUnreal(FVector(p3.x, p3.y, p3.z)));
				Vertices.Add(JoltPhysicsToUnreal(FVector(p2.x, p2.y, p2.z)));
				Vertices.Add(JoltPhysicsToUnreal(FVector(p1.x, p1.y, p1.z)));

				Triangles.Add(Vertices.Num() - 1);
				Triangles.Add(Vertices.Num() - 2);
				Triangles.Add(Vertices.Num() - 3);
			}
		} while (triangle_count > 0);

		DrawDebugMesh(World, Vertices, Triangles, Color, false, -1.0f, 0);

		FVector WorldPosition = FVector::ZeroVector;
		GetPosition(WorldPosition);

		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			const FVector WorldVel = GetLinearVelocityPerSecond();
			DrawDebugString(World, WorldPosition, FString::Printf(TEXT("Vel: %s"), *WorldVel.ToString()));
		}
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsConvexHullBody End

// FJPRPhysicsHeightFieldBody Begin
void FJPRPhysicsHeightFieldBody::InitHeightField(int32 SizeX, int32 SizeY, const TArray<float>& Heights, const FVector& Scale,
	const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	ensureAlwaysMsgf(SizeX == SizeY, TEXT("%hs Height Field should be a square"), __FUNCTION__);
	
	const int32 sample_count = FMath::Min(SizeX, SizeY);

	TArray<float> heights;
	heights.SetNum(Heights.Num());
	
	for (int y = 0; y < SizeY; ++y)
	{
		for (int x = 0; x < SizeX; ++x)
		{
			const int32 flippedY = sample_count - 1 - y;
			heights[y * SizeX + x] = Heights[flippedY * SizeX + x] * UnrealToJoltPhysicsUnitScale;
		}		
	}

	const FVector scale = UnrealToJoltPhysicsScale(Scale);

	// Create height field
	RefConst<ShapeSettings> height_field = new HeightFieldShapeSettings(heights.GetData(),
		Vec3(0.0f, 0.0f, -1.0f * scale.Z * sample_count), Vec3Arg(scale.X, scale.Y, scale.Z), sample_count);

	BodyCreationSettings body_creation_settings(height_field, RVec3::sZero(), Quat::sIdentity(),
		static_cast<EMotionType>(Params.MotionType), static_cast<ObjectLayer>(Layer));

	SetupBodyCreationSettings(body_creation_settings, Params);
	
	CreateAndSetBody(body_creation_settings, InBodyID);

	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsHeightFieldBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		ShapeRefC Shape = GetBodyInterface().GetShape(*body_id.Get());
		const HeightFieldShape* ConvexHullShapePtr = static_cast<const HeightFieldShape*>(Shape.GetPtr());
		if (ConvexHullShapePtr == nullptr)
		{
			return;
		}

		const Quat Rotation = GetBodyInterface().GetRotation(*body_id.Get());
		const RVec3 PositionCOM = GetBodyInterface().GetCenterOfMassPosition(*body_id.Get());

		// TODO: Debug draw
	}
#endif // WITH_JOLT_PHYSICS
}
// FJPRPhysicsHeightFieldBody End
