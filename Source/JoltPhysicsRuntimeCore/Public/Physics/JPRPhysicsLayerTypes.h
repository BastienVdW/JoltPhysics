// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

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
