// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "Physics/JPRPhysicsObjectFactory.h"

#include "Physics/JPRPhysicsBody.h"
#include "System/JPRPhysicsSubsystem.h"

TSharedPtr<FJPRPhysicsBody> UJPRPhysicsObjectFactory::BuildPhysicsObject(UObject* Outer, uint32 BodyID, const FInstancedStruct& Shape,
	const FJPRPhysicsBodyParameters& Params) const
{
	unimplemented();
	return nullptr;
}

void UJPRPhysicsObjectFactory::SetupPhysicsObject(UObject* Outer, const TSharedPtr<FJPRPhysicsBody>& Body) const
{
	if (!ensure(Body.IsValid()))
	{
		return;
	}

	Body->SetWorldContextObject(Outer);

#if WITH_JOLT_PHYSICS
	const UJPRPhysicsSubsystem* PhysicsSubsystem = CastChecked<UJPRPhysicsSubsystem>(Outer);
	Body->SetPhysicsSystem(PhysicsSubsystem->GetPhysicsSystemPtr());
	Body->SetTempAllocator(PhysicsSubsystem->GetTempAllocator());
#endif // WITH_JOLT_PHYSICS
}
