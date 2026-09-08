// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "JPRPhysicsObjectFactory.generated.h"

class FJPRPhysicsBody;
struct FJPRPhysicsBodyParameters;

/** Base factory for initializing Jolt Physics Runtime body wrappers. */
UCLASS(Abstract, Within=JPRPhysicsSubsystem)
class JOLTPHYSICSRUNTIME_API UJPRPhysicsObjectFactory : public UObject
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const;
	
protected:
	void SetupPhysicsObject(UObject* Outer, const TSharedPtr<FJPRPhysicsBody>& Body) const;
};
