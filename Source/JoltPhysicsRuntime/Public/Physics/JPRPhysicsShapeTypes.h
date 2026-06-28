// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

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
