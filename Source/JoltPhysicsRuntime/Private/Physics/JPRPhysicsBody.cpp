// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "Physics/JPRPhysicsBody.h"

FJPRPhysicsBody::FJPRPhysicsBody() = default;

FJPRPhysicsBody::FJPRPhysicsBody(const FJPRPhysicsBody& Other) = default;

FJPRPhysicsBody& FJPRPhysicsBody::operator=(const FJPRPhysicsBody& Other) = default;

FJPRPhysicsBody::~FJPRPhysicsBody() = default;

bool FJPRPhysicsBody::IsEnabled() const
{
	return bEnabled;
}

bool FJPRPhysicsBody::DoesTriggerHitEvents() const
{
	return bTriggerHitEvents;
}

void FJPRPhysicsBody::SetWorldContextObject(UObject* Object)
{
	WorldContextObject = Object;
}

UObject* FJPRPhysicsBody::GetWorldContextObject() const
{
	return WorldContextObject.Get();
}
