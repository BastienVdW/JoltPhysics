// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "Physics/JPRPhysicsShapeTypes.h"

#include "Physics/JPRPhysicsObjectFactory.h"

FJPRPhysicsShape::FJPRPhysicsShape(const TSubclassOf<UJPRPhysicsObjectFactory>& InFactoryClass)
	: FactoryClass(InFactoryClass)
{
}
