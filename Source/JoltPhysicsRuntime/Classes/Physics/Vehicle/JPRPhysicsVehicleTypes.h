// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Physics/JPRPhysicsTypes.h"

#include "JPRPhysicsVehicleTypes.generated.h"

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleWheelSettings
{
	GENERATED_BODY()

	/**
	 * Offset of the front wheels toward the center (0.0) or the front (1.0) of the vehicle.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, ClampMin="0.0"))
	float WheelFrontOffset = 150.0f;
	
	/**
	 * Offset of the rear wheels toward the center (0.0) or the rear (1.0) of the vehicle.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, ClampMin="0.0"))
	float WheelRearOffset = 150.0f;
	
	/**
	 * Offset of the wheels toward the center (0.0) or the side (1.0) of the vehicle.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters, ClampMin="0.0"))
	float WheelHorizontalOffset = 100.0f;
	
	/**
	 * Offset of the wheels toward the bottom (-1.0) or the top (1.0) of the vehicle.
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Centimeters))
	float WheelVerticalOffset = -50.0f;

	/**
	 * How long the suspension is in max raised position relative to the attachment point
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float					SuspensionMinLength = 30.0f;
	
	/**
	 * How long the suspension is in max droop position relative to the attachment point
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float					SuspensionMaxLength = 50.0f;
	
	/**
	 * The natural length of the suspension spring is defined as mSuspensionMaxLength + mSuspensionPreloadLength.
	 * Can be used to preload the suspension as the spring is compressed by mSuspensionPreloadLength when the suspension is in max droop position.
	 * Note that this means when the vehicle touches the ground there is a discontinuity so it will also make the vehicle more bouncy as we're updating with discrete time steps.
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float					SuspensionPreloadLength = 0.0f;

	/**
	 * Settings for the suspension spring
	 */
	UPROPERTY(EditAnywhere)
	FJPRPhysicsSpringSettings SuspensionSpring { EJPRPhysicsSpringMode::FrequencyAndDamping, 1.5f, 0.0f, 0.5f };
	
	/**
	 * Radius of the wheel
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float					Radius = 30.0f;
	
	/**
	 * Width of the wheel
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Centimeters"))
	float					Width = 10.0f;
	
	/**
	 * Enables mSuspensionForcePoint, if disabled, the forces are applied at the collision contact point.
	 * This leads to a more accurate simulation when interacting with dynamic objects but makes the vehicle less stable.
	 * When setting this to true, all forces will be applied to a fixed point on the vehicle body.
	 */
	UPROPERTY(EditAnywhere)
	bool					EnableSuspensionForcePoint = false;
	
	/**
	 * Moment of inertia (kg m^2), for a cylinder this would be 0.5 * M * R^2 which is 0.9 for a wheel with a mass of 20 kg and radius 0.3 m
	 */
	UPROPERTY(EditAnywhere)
	float					Inertia = 0.9f;
	
	/**
	 * Angular damping factor of the wheel: dw/dt = -c * w
	 */
	UPROPERTY(EditAnywhere)
	float					AngularDamping = 0.2f;
	
	/**
	 * How much this wheel can steer (radians)
	 */
	UPROPERTY(EditAnywhere, meta=(Units="Degrees"))
	float					MaxSteerAngle = 70.0f;
	
	/**
	 * How much torque (Nm) the brakes can apply to this wheel
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="Nm"))
	float					MaxBrakeTorque = 1500.0f;
	
	/**
	 * How much torque (Nm) the hand brake can apply to this wheel (usually only applied to the rear wheels)
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="Nm"))
	float					MaxHandBrakeTorque = 4000.0f;

	/**
	 * On the Y-axis: friction in the forward direction of the tire.
	 * Friction is normally between 0 (no friction) and 1 (full friction) although friction can be a little bit higher than 1 because of the profile of a tire.
	 * On the X-axis: the slip ratio (fraction) defined as (omega_wheel * r_wheel - v_longitudinal) / |v_longitudinal|.
	 * You can see slip ratio as the amount the wheel is spinning relative to the floor:
	 * 0 means the wheel has full traction and is rolling perfectly in sync with the ground,
	 * 1 is for example when the wheel is locked and sliding over the ground.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat>	LongitudinalFriction;
	
	/**
	 * On the Y-axis: friction in the sideways direction of the tire.
	 * Friction is normally between 0 (no friction) and 1 (full friction) although friction can be a little bit higher than 1 because of the profile of a tire.
	 * On the X-axis: the slip angle (degrees) defined as angle between relative contact velocity and tire direction.
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat>	LateralFriction;

	/**
	 * Multiplier applied to the curve values (Y-axis).
	 * Does not apply if no curve is set.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float LongitudinalFrictionScale = 1.0f;

	/**
	 * Multiplier applied to the curve values (Y-axis).
	 * Does not apply if no curve is set.
	 */
	UPROPERTY(EditAnywhere, meta=(ClampMin="0.0"))
	float LateralFrictionScale = 1.0f;

};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleEngineSettings
{
	GENERATED_BODY()

	/**
	 * Max amount of torque (Nm) that the engine can deliver
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="Nm"))
	float					MaxTorque = 500.0f;

	/**
	 * Min amount of revolutions per minute (rpm) the engine can produce without stalling
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="rpm"))
	float					MinRPM = 1000.0f;
	
	/**
	 * Max amount of revolutions per minute (rpm) the engine can generate
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="rpm"))
	float					MaxRPM = 6000.0f;

	/**
	 * Y-axis: Curve that describes a ratio of the max torque the engine can produce (0 = 0, 1 = mMaxTorque).
	 * X-axis: the fraction of the RPM of the engine (0 = mMinRPM, 1 = mMaxRPM)
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat>	NormalizedTorque;

	/**
	 * Moment of inertia (kg m^2) of the engine
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="kg m^2"))
	float					Inertia = 0.5f;

	/**
	 * Angular damping factor of the wheel: dw/dt = -c * w
	 */
	UPROPERTY(EditAnywhere)
	float					AngularDamping = 0.2f;
};

UENUM()
enum class EJPRPhysicsVehicleTransmissionMode : uint8
{
	/**
	 * Automatically shift gear up and down
	 */
	Auto,

	/**
	 * Manual gear shift (call SetTransmissionInput)
	 */	
	Manual,
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleTransmissionSettings
{
	GENERATED_BODY()

	/**
	 * How to switch gears
	 */
	UPROPERTY(EditAnywhere)
	EJPRPhysicsVehicleTransmissionMode		Mode = EJPRPhysicsVehicleTransmissionMode::Auto;
	
	/**
	 * Ratio in rotation rate between engine and gear box, first element is 1st gear, 2nd element 2nd gear etc.
	 */
	UPROPERTY(EditAnywhere)
	TArray<float>			GearRatios { 2.66f, 1.78f, 1.3f, 1.0f, 0.74f };

	/**
	 * Ratio in rotation rate between engine and gear box when driving in reverse
	 */
	UPROPERTY(EditAnywhere)
	TArray<float>			ReverseGearRatios { -2.90f };

	/**
	 * How long it takes to switch gears (s), only used in auto mode
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Seconds))
	float					SwitchTime = 0.5f;

	/**
	 * How long it takes to release the clutch (go to full friction), only used in auto mode
	 */
	UPROPERTY(EditAnywhere)
	float					ClutchReleaseTime = 0.3f;

	/**
	 * How long to wait after releasing the clutch before another switch is attempted (s), only used in auto mode
	 */
	UPROPERTY(EditAnywhere, meta=(Units=Seconds))
	float					SwitchLatency = 0.5f;

	/**
	 * If RPM of engine is bigger then this we will shift a gear up, only used in auto mode
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="rpm"))
	float					ShiftUpRPM = 4000.0f;

	/**
	 * If RPM of engine is smaller then this we will shift a gear down, only used in auto mode
	 */
	UPROPERTY(EditAnywhere, meta=(ForceUnits="rpm"))
	float					ShiftDownRPM = 2000.0f;

	/**
	 * Strength of the clutch when fully engaged.
	 * Total torque a clutch applies is Torque = ClutchStrength * (Velocity Engine - Avg Velocity Wheels At Clutch) (units: k m^2 s^-1)
	 */
	UPROPERTY(EditAnywhere)
	float					ClutchStrength = 10.0f;
};

USTRUCT()
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleDifferentialSettings
{
	GENERATED_BODY()

	/**
	 * Index (in mWheels) that represents the left wheel of this differential (can be -1 to indicate no wheel)
	 */
	UPROPERTY(EditAnywhere)
	int						LeftWheel = INDEX_NONE;
	
	/**
	 * Index (in mWheels) that represents the right wheel of this differential (can be -1 to indicate no wheel)
	 */
	UPROPERTY(EditAnywhere)
	int						RightWheel = INDEX_NONE;

	/**
	 * Ratio between rotation speed of gear box and wheels
	 */
	UPROPERTY(EditAnywhere)
	float					DifferentialRatio = 3.42f;

	/**
	 * Defines how the engine torque is split across the left and right wheel (0 = left, 0.5 = center, 1 = right)
	 */
	UPROPERTY(EditAnywhere)
	float					LeftRightSplit = 0.5f;
	
	/**
	 * Ratio max / min wheel speed. When this ratio is exceeded, all torque gets distributed to the slowest moving wheel.
	 * This allows implementing a limited slip differential. Set to FLT_MAX for an open differential. Value should be > 1.
	 */
	UPROPERTY(EditAnywhere)
	float					LimitedSlipRatio = 1.4f;
	
	/**
	 * How much of the engines torque is applied to this differential (0 = none, 1 = full), make sure the sum of all differentials is 1.
	 */
	UPROPERTY(EditAnywhere)
	float					EngineTorqueRatio = 1.0f;
};

USTRUCT(BlueprintType)
struct JOLTPHYSICSRUNTIME_API FJPRPhysicsVehicleWheelContact
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasContact = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Position = FVector::ZeroVector;
};
