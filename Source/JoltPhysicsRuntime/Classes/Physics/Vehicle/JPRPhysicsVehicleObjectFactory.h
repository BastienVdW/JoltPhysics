// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "Physics/JPRPhysicsObjectFactory.h"

#include "JPRPhysicsVehicleObjectFactory.generated.h"

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsVehicleObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};
