// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "JPRPhysicsCharacterShapeTypes.h"

#include "JPRPhysicsCharacterObjectFactory.h"

FJPRPhysicsCharacterCapsule::FJPRPhysicsCharacterCapsule()
{
}

FJPRPhysicsCharacterCapsule::FJPRPhysicsCharacterCapsule(float InRadius, float InHalfHeightOfCylinder)
	: Radius(InRadius)
	, HalfHeightOfCylinder(InHalfHeightOfCylinder)
{
}

FJPRPhysicsCharacter::FJPRPhysicsCharacter()
	: Super(UJPRPhysicsCharacterObjectFactory::StaticClass())
{
}

FJPRPhysicsCharacterVirtual::FJPRPhysicsCharacterVirtual()
	: Super(UJPRPhysicsCharacterVirtualObjectFactory::StaticClass())
{
}
