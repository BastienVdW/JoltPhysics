// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

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
