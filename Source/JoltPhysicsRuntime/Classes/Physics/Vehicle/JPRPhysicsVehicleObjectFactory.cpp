// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "JPRPhysicsVehicleObjectFactory.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "JPRPhysicsVehicleObject.h"
#include "JPRPhysicsVehicleShapeTypes.h"

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsVehicleObjectFactory::BuildPhysicsObject(UObject* Outer,
	uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsVehicleShape& VehicleShape = Shape.Get<FJPRPhysicsVehicleShape>();
	const TSharedPtr<FJPRPhysicsVehicleBody> VehicleBody = MakeShared<FJPRPhysicsVehicleBody>();
	
	SetupPhysicsObject(Outer, VehicleBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	VehicleBody->InitVehicle(VehicleShape, Params, BodyID, Layer);
	
	return VehicleBody;
}
