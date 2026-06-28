// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

#include "JPRPhysicsObjectFactory.generated.h"

class FJPRPhysicsBody;

/** Base factory for initializing Jolt Physics Runtime body wrappers. */
UCLASS(Abstract, Within=JPRPhysicsSubsystem)
class JOLTPHYSICSRUNTIME_API UJPRPhysicsObjectFactory : public UObject
{
	GENERATED_BODY()

protected:
	void SetupPhysicsObject(const TSharedPtr<FJPRPhysicsBody>& Body) const;
};
