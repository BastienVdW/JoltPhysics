// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


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
