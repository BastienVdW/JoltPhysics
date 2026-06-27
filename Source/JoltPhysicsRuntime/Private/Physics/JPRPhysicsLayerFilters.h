// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0

#pragma once

#include "Templates/SharedPointer.h"

class UJPRPhysicsLayerDataAsset;

namespace JPH
{
	class BroadPhaseLayerInterface;
	class ObjectLayerPairFilter;
	class ObjectVsBroadPhaseLayerFilter;
}

namespace JPR::Private
{
	TSharedPtr<JPH::BroadPhaseLayerInterface> CreateBroadPhaseLayerInterface(const UJPRPhysicsLayerDataAsset& PhysicsLayer);
	TSharedPtr<JPH::ObjectLayerPairFilter> CreateObjectLayerPairFilter(const UJPRPhysicsLayerDataAsset& PhysicsLayer);
	TSharedPtr<JPH::ObjectVsBroadPhaseLayerFilter> CreateObjectVsBroadPhaseLayerFilter(const UJPRPhysicsLayerDataAsset& PhysicsLayer);
}
