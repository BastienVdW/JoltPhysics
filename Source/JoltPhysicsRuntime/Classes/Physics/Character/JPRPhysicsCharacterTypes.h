// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

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
