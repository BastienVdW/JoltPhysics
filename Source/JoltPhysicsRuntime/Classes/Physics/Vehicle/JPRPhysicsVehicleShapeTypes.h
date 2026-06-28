// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "JPRPhysicsVehicleTypes.h"
#include "Physics/JPRPhysicsShapeTypes.h"

#include "JPRPhysicsVehicleShapeTypes.generated.h"

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleShape : public FJPRPhysicsShape
{
	GENERATED_BODY()

	FJPRPhysicsVehicleShape();
	
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float Width = 180.0f;
	
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float Height = 40.0f;

	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float Length = 400.0f;

	/**
	 * Defines the maximum pitch/roll angle, can be used to avoid the car from getting upside down.
	 * The vehicle up direction will stay within a cone centered around the up axis with half top angle mMaxPitchRollAngle, set to pi to turn off.
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Degrees"))
	float MaxPitchRollAngle = 60.0f;

	/*
	 * Wheel settings shared by all the wheels.
	 */
	UPROPERTY(EditAnywhere)
	FJPRPhysicsVehicleWheelSettings Wheels;

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FJPRPhysicsVehicleEngineSettings Engine;

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FJPRPhysicsVehicleTransmissionSettings Transmission;

	UPROPERTY(EditAnywhere)
	TArray<FJPRPhysicsVehicleDifferentialSettings> Differentials;

	/**
	 * Number of simulation steps between wheel collision tests when the vehicle is active.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0))
	int32 NumStepsBetweenCollisionTestActive = 1;

	/**
	 * Number of simulation steps between wheel collision tests when the vehicle is inactive.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin=0))
	int32 NumStepsBetweenCollisionTestInactive = 1;
};
