// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "Physics/JPRPhysicsObjectFactory.h"

#include "JPRPhysicsCharacterObjectFactory.generated.h"

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsCharacterObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsCharacterVirtualObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};
