// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "Settings/JPRPhysicsSettings.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "UObject/ConstructorHelpers.h"

UJPRPhysicsSettings::UJPRPhysicsSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	static ConstructorHelpers::FObjectFinder<UJPRPhysicsLayerDataAsset> DefaultLayerAsset(TEXT("/JoltPhysics/DataAssets/Physics/DA_PhysicsLayer_Default"));
	DefaultLayer = DefaultLayerAsset.Object;
}
