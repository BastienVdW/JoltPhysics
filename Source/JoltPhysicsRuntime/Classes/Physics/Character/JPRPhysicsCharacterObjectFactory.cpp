// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT


#include "JPRPhysicsCharacterObjectFactory.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"
#include "JPRPhysicsCharacterObject.h"
#include "JPRPhysicsCharacterShapeTypes.h"
#include "JPRPhysicsCharacterVirtualObject.h"
#include "Physics/JPRPhysicsTypes.h"

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsCharacterObjectFactory::BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsCharacter& CharacterShape = Shape.Get<FJPRPhysicsCharacter>();
	const TSharedPtr<FJPRPhysicsCharacterBody> CharacterBody = MakeShared<FJPRPhysicsCharacterBody>();
	
	SetupPhysicsObject(Outer, CharacterBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	CharacterBody->InitCharacter(CharacterShape, Params, BodyID, Layer);
	
	return CharacterBody;
}

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsCharacterVirtualObjectFactory::BuildPhysicsObject(UObject* Outer,
	uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const
{
	const FJPRPhysicsCharacterVirtual& CharacterVirtualShape = Shape.Get<FJPRPhysicsCharacterVirtual>();
	const TSharedPtr<FJPRPhysicsCharacterVirtualBody> CharacterBody = MakeShared<FJPRPhysicsCharacterVirtualBody>();
	
	SetupPhysicsObject(Outer, CharacterBody);
	
	const int32 Layer = UJPRPhysicsLayerDataAsset::GetLayerIndex(Params.Layer);
	CharacterBody->InitCharacterVirtual(CharacterVirtualShape, Params, BodyID, Layer);
	
	return CharacterBody;
}
