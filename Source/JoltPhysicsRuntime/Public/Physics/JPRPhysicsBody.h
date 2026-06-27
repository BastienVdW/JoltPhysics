// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"

/**
 * Unreal-facing base for a body owned by the Jolt runtime wrapper.
 *
 * Raw Jolt types intentionally do not appear in this public interface. More body behavior will move
 * here incrementally while Recall keeps its existing type as a compatibility shell.
 */
class JOLTPHYSICSRUNTIME_API FJPRPhysicsBody
{
public:
	FJPRPhysicsBody();
	FJPRPhysicsBody(const FJPRPhysicsBody& Other);
	FJPRPhysicsBody& operator=(const FJPRPhysicsBody& Other);
	virtual ~FJPRPhysicsBody();

	bool IsEnabled() const;
	bool DoesTriggerHitEvents() const;

	void SetWorldContextObject(UObject* Object);
	UObject* GetWorldContextObject() const;

protected:
	bool bEnabled = false;
	bool bTriggerHitEvents = true;

private:
	TWeakObjectPtr<UObject> WorldContextObject;
};
