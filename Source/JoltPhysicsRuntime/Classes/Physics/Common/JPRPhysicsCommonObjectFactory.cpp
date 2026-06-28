// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "JPRPhysicsCommonObjectFactory.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "JPRPhysicsCommonObjects.h"
#include "JPRPhysicsCommonShapeTypes.h"
#include "Physics/JPRPhysicsTypes.h"

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsBoxObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsBoxShape& BoxShape = Shape.Get<FJPRPhysicsBoxShape>();
	const TSharedPtr<FJPRPhysicsBoxBody> BoxBody = MakeShared<FJPRPhysicsBoxBody>();
	
	SetupPhysicsObject(Outer, BoxBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	BoxBody->InitBox(BoxShape.WorldExtents, Params, BodyID, Layer);
	
	return BoxBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsSphereObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsSphereShape& SphereShape = Shape.Get<FJPRPhysicsSphereShape>();
	const TSharedPtr<FJPRPhysicsSphereBody> SphereBody = MakeShared<FJPRPhysicsSphereBody>();
	
	SetupPhysicsObject(Outer, SphereBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	SphereBody->InitSphere(SphereShape.RadiusCentimeters, Params, BodyID, Layer);
	
	return SphereBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsCapsuleObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsCapsuleShape& CapsuleShape = Shape.Get<FJPRPhysicsCapsuleShape>();
	const TSharedPtr<FJPRPhysicsCapsuleBody> CapsuleBody = MakeShared<FJPRPhysicsCapsuleBody>();
	
	SetupPhysicsObject(Outer, CapsuleBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	CapsuleBody->InitCapsule(CapsuleShape.RadiusCentimeters, CapsuleShape.HalfHeightCentimeters, Params, BodyID, Layer);
	
	return CapsuleBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsMeshObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsMeshShape& MeshShape = Shape.Get<FJPRPhysicsMeshShape>();
	const TSharedPtr<FJPRPhysicsMeshBody> MeshBody = MakeShared<FJPRPhysicsMeshBody>();
	
	SetupPhysicsObject(Outer, MeshBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	MeshBody->InitMesh(MeshShape, Params, BodyID, Layer);
	
	return MeshBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsConvexHullObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsConvexHullShape& ConvexHullShape = Shape.Get<FJPRPhysicsConvexHullShape>();
	const TSharedPtr<FJPRPhysicsConvexHullBody> ConvexHullBody = MakeShared<FJPRPhysicsConvexHullBody>();
	
	SetupPhysicsObject(Outer, ConvexHullBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	ConvexHullBody->InitConvexHull(ConvexHullShape.Vertices, Params, BodyID, Layer);
	
	return ConvexHullBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsHeightFieldObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsHeightFieldShape& HeightFieldShape = Shape.Get<FJPRPhysicsHeightFieldShape>();
	const TSharedPtr<FJPRPhysicsHeightFieldBody> HeightFieldBody = MakeShared<FJPRPhysicsHeightFieldBody>();
	
	SetupPhysicsObject(Outer, HeightFieldBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	HeightFieldBody->InitHeightField(HeightFieldShape.SizeX, HeightFieldShape.SizeY, HeightFieldShape.Heights,
		HeightFieldShape.Scale, Params, BodyID, Layer);
	
	return HeightFieldBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsStaticCompoundObjectFactory::BuildPhysicsObject(UObject* Outer,
	uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsStaticCompoundShape& StaticCompoundShape = Shape.Get<FJPRPhysicsStaticCompoundShape>();
	const TSharedPtr<FJPRPhysicsStaticCompoundBody> StaticCompoundBody = MakeShared<FJPRPhysicsStaticCompoundBody>();
	
	SetupPhysicsObject(Outer, StaticCompoundBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	StaticCompoundBody->InitStaticCompound(StaticCompoundShape, Params, BodyID, Layer);
	
	return StaticCompoundBody;
}
