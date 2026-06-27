// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "JPRPhysicsLayerTypes.generated.h"

USTRUCT(BlueprintType)
struct JOLTPHYSICSRUNTIMECORE_API FJPRPhysicsLayerTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FText LayerDescription;
};
