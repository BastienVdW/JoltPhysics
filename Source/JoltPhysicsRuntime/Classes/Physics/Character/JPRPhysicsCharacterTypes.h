// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EJPRPhysicsCharacterGroundState : uint8
{
	/// Character is on the ground and can move freely.
	OnGround,
	
	/// Character is on a slope that is too steep and can't climb up any further. The caller should start applying downward velocity if sliding from the slope is desired.
	OnSteepGround,
	
	/// Character is touching an object, but is not supported by it and should fall. The GetGroundXXX functions will return information about the touched object.
	NotSupported,

	/// Character is in the air and is not touching anything.
	InAir,
};
