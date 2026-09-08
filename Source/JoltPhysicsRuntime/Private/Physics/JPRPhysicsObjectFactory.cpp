// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

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
