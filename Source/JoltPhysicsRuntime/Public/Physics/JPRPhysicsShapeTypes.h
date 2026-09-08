// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"

#include "JPRPhysicsShapeTypes.generated.h"

class UJPRPhysicsObjectFactory;

/**
 * Base struct of a physics shape that can be used to build a physics object.
 */
USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsShape
{
	GENERATED_BODY()
	
	FJPRPhysicsShape() = default;
	FJPRPhysicsShape(const TSubclassOf<UJPRPhysicsObjectFactory>& InFactoryClass);

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UJPRPhysicsObjectFactory> FactoryClass;
};
