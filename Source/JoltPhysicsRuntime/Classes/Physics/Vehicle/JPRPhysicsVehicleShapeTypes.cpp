// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "JPRPhysicsVehicleShapeTypes.h"

#include "JPRPhysicsVehicleObjectFactory.h"

FJPRPhysicsVehicleShape::FJPRPhysicsVehicleShape()
	: Super(UJPRPhysicsVehicleObjectFactory::StaticClass())
{
	Differentials.SetNum(1);
	Differentials[0].LeftWheel = 0;
	Differentials[0].RightWheel = 1;
}
