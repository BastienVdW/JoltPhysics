// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Physics/JPRPhysicsBody.h"
#include "JPRPhysicsCharacterTypes.h"

struct FJPRPhysicsCharacterVirtual;

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class CharacterVirtual;
	class CapsuleShape;
} // namespace JPH
#endif // WITH_JOLT_PHYSICS

/**
* Wrapper Object for JPH::CharacterVirtual.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsCharacterVirtualBody : public FJPRPhysicsBody
{
public:
	void InitCharacterVirtual(const FJPRPhysicsCharacterVirtual& CharacterVirtualShape,
		const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);
	void Update(float DeltaTime);

	bool IsSupported() const;
	EJPRPhysicsCharacterGroundState GetGroundState() const;

	bool SetStance(bool bStand);
	bool IsStanding() const { return bIsStanding; }
	
public:
	virtual void Activate() override;
	virtual void Desactivate() override;
	
	virtual void AddLinearVelocityPerSecond(const FVector& LinearVelocity) override;
	virtual void SetLinearVelocityPerSecond(const FVector& LinearVelocity) override;
	virtual FVector GetLinearVelocityPerSecond() const override;
	virtual void SetPosition(const FVector& Position) const override;
	virtual void SetRotation(const FQuat& Rotation) const override;
	virtual void SetPositionAndRotation(const FVector& Position, const FQuat& Rotation) const override;
	virtual void GetPositionAndRotation(FVector& OutPosition, FQuat& OutRotation) const override;

	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;

protected:
	virtual void ReleasePhysicsObject() override;

#if WITH_JOLT_PHYSICS
	virtual void SaveState(JPH::StateRecorder& State) override;
	virtual void RestoreState(JPH::StateRecorder& State) override;

private:
	TSharedPtr<JPH::CharacterVirtual> character;
	int32 Layer = 0;
#endif // WITH_JOLT_PHYSICS

private:
	bool bIsStanding = true;

	float StandOffset = 0.0f;
	TSharedPtr<struct FJPRPhysicsCharacterVirtualBodyShapeContainer> ShapeContainer;

	bool SwitchStance(bool bStand, float MaxPenetrationDepth, float Offset = 0.0f);
};
