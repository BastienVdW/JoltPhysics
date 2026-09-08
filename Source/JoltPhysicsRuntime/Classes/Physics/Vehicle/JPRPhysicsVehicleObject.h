// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "JPRPhysicsVehicleShapeTypes.h"
#include "Physics/JPRPhysicsBody.h"
#include "JPRPhysicsVehicleTypes.h"

#if WITH_JOLT_PHYSICS
namespace JPH
{
	class VehicleConstraint;
} // namespace JPH
#endif // WITH_JOLT_PHYSICS

/**
* Wrapper Object for JPH::VehicleConstraintSettings.
*/
class JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleBody : public FJPRPhysicsBody
{
public:
	void InitVehicle(const FJPRPhysicsVehicleShape& VehicleShape, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer);
	
	void SetDriverInput(float Forward, float Right, float Brake, float HandBrake);
	void GetDriverInput(float& OutForward, float& OutRight, float& OutBrake, float& OutHandBrake) const;
	
	FVector GetSpeedKilometersPerHour(const UWorld* World) const;
	TArray<FJPRPhysicsVehicleWheelContact> GetWheelContacts() const;
	bool HasRearWheelsContact() const;
	
public:
	virtual void DrawDebugShape(const UWorld* World, const FColor& Color) const override;
	virtual void DrawDebugInfo(const UWorld* World, const FColor& Color) const override;

protected:
	virtual void ReleasePhysicsObject() override;
	
#if WITH_JOLT_PHYSICS	
private:
	TSharedPtr<struct FJPRVehicleRef> vehicle_constrain;
#endif // WITH_JOLT_PHYSICS
};
