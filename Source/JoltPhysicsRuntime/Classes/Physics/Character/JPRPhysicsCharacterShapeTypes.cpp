// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


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
