// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "JPRPhysicsTypes.generated.h"

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EJPRPhysicsTransformCopyParameters : uint8
{
	None					= 0,
	CopyLocation			= 1 << 0,
	CopyRotation			= 1 << 1,
	
	All						= 0xFF,

	CopyLocationAndRotation = CopyLocation | CopyRotation,
};
ENUM_CLASS_FLAGS(EJPRPhysicsTransformCopyParameters)

constexpr bool EnumHasAnyFlags(uint8 Flags, EJPRPhysicsTransformCopyParameters Contains) { return (Flags & static_cast<uint8>(Contains)) != 0; }
inline uint8& operator|=(uint8& Lhs, EJPRPhysicsTransformCopyParameters Rhs) { return Lhs |= static_cast<uint8>(Rhs); }

/// Enum used by AddBody to determine if the body needs to be initially active
UENUM()
enum class EJPRPhysicsActivation : uint8
{
	// Activate the body, making it part of the simulation
	Activate,	

	// Leave activation state as it is (will not deactivate an active body)
	DontActivate,				

	MAX						UMETA(Hidden),
};

UENUM()
enum class EJPRPhysicsMotionQuality : uint8
{
	/// Update the body in discrete steps. Body will tunnel throuh thin objects if its velocity is high enough.
	/// This is the cheapest way of simulating a body.
	Discrete,

	/// Update the body using linear casting. When stepping the body, its collision shape is cast from
	/// start to destination using the starting rotation. The body will not be able to tunnel through thin
	/// objects at high velocity, but tunneling is still possible if the body is long and thin and has high
	/// angular velocity. Time is stolen from the object (which means it will move up to the first collision
	/// and will not bounce off the surface until the next integration step). This will make the body appear
	/// to go slower when it collides with high velocity. In order to not get stuck, the body is always
	/// allowed to move by a fraction of it's inner radius, which may eventually lead it to pass through geometry.
	///
	/// Note that if you're using a collision listener, you can receive contact added/persisted notifications of contacts
	/// that may in the end not happen. This happens between bodies that are using casting: If bodies A and B collide at t1
	/// and B and C collide at t2 where t2 < t1 and A and C don't collide. In this case you may receive an incorrect contact
	/// point added callback between A and B (which will be removed the next frame).
	LinearCast,

	MAX						UMETA(Hidden),
};

UENUM()
enum class EJPRPhysicsMotionType : uint8
{
	// Non movable
	Static,			

	// Movable using velocities only, does not respond to forces
	Kinematic,		

	// Responds to forces as a normal physics object
	Dynamic,					

	MAX						UMETA(Hidden),
};

UENUM(meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EJPRPhysicsAllowedDOFs : uint8
{
	/**
	 * No degrees of freedom are allowed. Note that this is not valid and will crash. Use a static body instead.
	 */
	None				= 0b000000		UMETA(Hidden),

	/**
	 * All degrees of freedom are allowed
	 */
	All					= 0b111111,				

	/**
	 * Body can move in world space X axis
	 */
	TranslationX		= 0b000001,		

	/**
	 * Body can move in world space Y axis
	 */
	TranslationY		= 0b000010,			

	/**
	 * Body can move in world space Z axis
	 */
	TranslationZ		= 0b000100,		

	/**
	 * Body can rotate around world space X axis
	 */
	RotationX			= 0b001000,									
	
	/**
	 * Body can rotate around world space Y axis
	 */
	RotationY			= 0b010000,

	/**
	 * Body can rotate around world space Z axis
	 */
	RotationZ			= 0b100000,			

	/**
	 * Body can only move in X and Y axis and rotate around Z axis
	 */
	Plane2D				= TranslationX | TranslationY | RotationZ,
	
	/**
	 * Body can only move, but not rotate.
	 */
	MoveOnly			= TranslationX | TranslationY | TranslationZ,	
};
ENUM_CLASS_FLAGS(EJPRPhysicsAllowedDOFs)

/**
 * Enum used in BodyCreationSettings to indicate how mass and inertia should be calculated
 */
UENUM()
enum class EJPRPhysicsOverrideMassProperties : uint8
{
	/**
	 * Tells the system to calculate the mass and inertia based on density
	 */
	CalculateMassAndInertia,
	
	/**
	 * Tells the system to take the mass from mMassPropertiesOverride and to calculate the inertia based on density of the shapes and to scale it to the provided mass
	 */
	CalculateInertia,
	
	/**
	 * Tells the system to take the mass and inertia from mMassPropertiesOverride
	 */
	MassAndInertiaProvided,
};

USTRUCT()
struct JOLTPHYSICSRUNTIMECORE_API FJPRPhysicsMassProperties
{
	GENERATED_BODY()

	
	/// Mass of the shape (kg)
	UPROPERTY(EditAnywhere, meta=(Units="Kilograms"))
	float					Mass = 0.0f;

	/// Inertia tensor of the shape (kg m^2)
	UPROPERTY(EditAnywhere, meta=(ForceUnits="kg m^2"))
	float					Inertia = 0.0f;
	
	bool operator==(const FJPRPhysicsMassProperties& Other) const
	{
		return Mass == Other.Mass
			&& Inertia == Other.Inertia;
	}
};

USTRUCT(BlueprintType)
struct JOLTPHYSICSRUNTIMECORE_API FJPRPhysicsBodyParameters
{
	GENERATED_BODY()

public:
	// Motion type, determines if the object is static, dynamic or kinematic
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EJPRPhysicsMotionType MotionType{ EJPRPhysicsMotionType::Static };

	// Which degrees of freedom this body has (can be used to limit simulation to 2D)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Bitmask, BitmaskEnum="/Script/JoltPhysicsRuntimeCore.EJPRPhysicsAllowedDOFs"))
	uint8 AllowedDOFs = 0b111111;

	// The collision layer this body belongs to (determines if two objects can collide)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="/Script/JoltPhysicsRuntimeCore.JPRPhysicsLayerTableRow"))
	FDataTableRowHandle Layer;

	// Motion quality, or how well it detects collisions when it has a high velocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EJPRPhysicsMotionQuality MotionQuality{ EJPRPhysicsMotionQuality::Discrete };

	/**
	 * Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal edges of a mesh).
	 * This is more expensive but makes bodies move smoother over a mesh with convex edges.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool EnhancedInternalEdgeRemoval = false;

	// If this body can go to sleep or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowSleeping = true;

	// Value to multiply gravity with for this body
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GravityFactor{ 1.0f };

	// Friction of the body (dimensionless number, usually between 0 and 1, 0 = no friction, 1 = friction force equals force that presses the two bodies together).
	// Note that bodies can have negative friction but the combined friction (see PhysicsSystem::SetCombineFriction) should never go below zero.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Friction{ 0.2f };

	/**
	 * Jolt Note:
	 * Restitution of body (dimensionless number,
	 * usually between 0 and 1, 0 = completely inelastic collision
	 * response, 1 = completely elastic collision response)
	 *
	 * MS Note:
	 * This affects how much of its energy is retained when impacting another physical surface
	 * Example: 1.0 means 100% energy retained and will bounce back to exactly where it started
	 * 0.2 means 20% energy retained, so it will bounce up to 20% of the distance of where it started
	 */
	UPROPERTY(EditAnywhere)
	float Restitution { 0.0f };

	/**
	 * Jolt Note:
	 * Maximum angular velocity that this body can reach
	 * Jolt Default Value:  RadiansToDegrees(0.25f * JPH_PI * 60.0f) ~= 2,700.0
	 *
	 * MS Note:
	 * We have this in Degrees to make it easier to conceptualize
	 */
	UPROPERTY(EditAnywhere)
	float MaxAngularVelocityDegrees { 2700.0f };
	
	/* Should body that hit this body trigger hit event or not. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTriggerHitEvents{ true };

	/**
	 * Jolt Note:
	 * When this body is created as static,
	 * this setting tells the system to create a MotionProperties
	 * object so that the object can be switched to kinematic or dynamic
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowDynamicOrKinematic{ false };

	/**
	 * If this body is a sensor. A sensor will receive collision callbacks, but will
	 * not cause any collision responses and can be used as a trigger volume.
	 * See description at Body::SetIsSensor.
	 */
	UPROPERTY(EditAnywhere)
	bool bIsSensor { false };

	/**
	 * Determines how mMassPropertiesOverride will be used
	 */
	UPROPERTY(EditAnywhere)
	EJPRPhysicsOverrideMassProperties OverrideMassProperties = EJPRPhysicsOverrideMassProperties::CalculateMassAndInertia;

	/**
	 * When calculating the inertia (not when it is provided) the calculated inertia will be multiplied by this value
	 */
	UPROPERTY(EditAnywhere)
	float InertiaMultiplier = 1.0f;

	/**
	 * Contains replacement mass settings which override the automatically calculated value
	 */
	UPROPERTY(EditAnywhere)
	FJPRPhysicsMassProperties MassPropertiesOverride;
	
	/// Used only when this body is dynamic and colliding.
	/// Override for the number of solver velocity iterations to run, 0 means use the default in PhysicsSettings::mNumVelocitySteps.
	/// The number of iterations to use is the max of all contacts and constraints in the island.
	UPROPERTY(EditAnywhere)
	int32 NumVelocityStepsOverride = 0;
	
	/// Used only when this body is dynamic and colliding.
	/// Override for the number of solver position iterations to run, 0 means use the default in PhysicsSettings::mNumPositionSteps.
	/// The number of iterations to use is the max of all contacts and constraints in the island.
	UPROPERTY(EditAnywhere)
	int32 NumPositionStepsOverride = 0;
	
	bool operator==(const FJPRPhysicsBodyParameters& Other) const
	{
		return MotionType == Other.MotionType
			&& AllowedDOFs == Other.AllowedDOFs
			&& Layer == Other.Layer
			&& MotionQuality == Other.MotionQuality
			&& EnhancedInternalEdgeRemoval == Other.EnhancedInternalEdgeRemoval
			&& bAllowSleeping == Other.bAllowSleeping
			&& GravityFactor == Other.GravityFactor
			&& Friction == Other.Friction
			&& Restitution == Other.Restitution
			&& MaxAngularVelocityDegrees == Other.MaxAngularVelocityDegrees
			&& bTriggerHitEvents == Other.bTriggerHitEvents
			&& bAllowDynamicOrKinematic == Other.bAllowDynamicOrKinematic
			&& bIsSensor == Other.bIsSensor
			&& OverrideMassProperties == Other.OverrideMassProperties
			&& InertiaMultiplier == Other.InertiaMultiplier
			&& MassPropertiesOverride == Other.MassPropertiesOverride
			&& NumVelocityStepsOverride == Other.NumVelocityStepsOverride
			&& NumPositionStepsOverride == Other.NumPositionStepsOverride;
	}

	FORCEINLINE bool IsStatic() const { return MotionType == EJPRPhysicsMotionType::Static && !bAllowDynamicOrKinematic; }
};

/**
 * Enum used by constraints to specify how the spring is
 */
UENUM()
enum class EJPRPhysicsSpringMode : uint8
{
	/**
	 * Frequency and damping are specified
	 */
	FrequencyAndDamping,

	/**
	 * Stiffness and damping are specified
	 */
	StiffnessAndDamping,
};

USTRUCT()
struct JOLTPHYSICSRUNTIMECORE_API FJPRPhysicsSpringSettings
{
	GENERATED_BODY()
	
	/**
	 * Selects the way in which the spring is defined
	 * If the mode is StiffnessAndDamping then mFrequency becomes the stiffness (k) and mDamping becomes the damping ratio (c) in the spring equation F = -k * x - c * v.
	 * Otherwise the properties are as documented.
	 */
	UPROPERTY(EditAnywhere)
	EJPRPhysicsSpringMode		Mode = EJPRPhysicsSpringMode::FrequencyAndDamping;

	/**
	 * If mFrequency > 0 the constraint will be soft and mFrequency specifies the oscillation frequency in Hz.
	 * If mFrequency <= 0, mDamping is ignored and the constraint will have hard limits (as hard as the time step / the number of velocity / position solver steps allows).
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="Mode == EJPRPhysicsSpringMode::FrequencyAndDamping", EditConditionHides))
	float							Frequency = 0.0f;
	
	/**	
	 * If mStiffness > 0 the constraint will be soft and mStiffness specifies the stiffness (k) in the spring equation F = -k * x - c * v for a linear or T = -k * theta - c * w for an angular spring.
	 * If mStiffness <= 0, mDamping is ignored and the constraint will have hard limits (as hard as the time step / the number of velocity / position solver steps allows).
	 *
	 * Note that stiffness values are large numbers. To calculate a ballpark value for the needed stiffness you can use:
	 * force = stiffness * delta_spring_length = mass * gravity <=> stiffness = mass * gravity / delta_spring_length.
	 * So if your object weighs 1500 kg and the spring compresses by 2 meters, you need a stiffness in the order of 1500 * 9.81 / 2 ~ 7500 N/m.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="Mode == EJPRPhysicsSpringMode::StiffnessAndDamping", EditConditionHides))
	float							Stiffness = 0.0f;

	/**
	 * When mSpringMode = ESpringMode::FrequencyAndDamping mDamping is the damping ratio (0 = no damping, 1 = critical damping).
	 * When mSpringMode = ESpringMode::StiffnessAndDamping mDamping is the damping (c) in the spring equation F = -k * x - c * v for a linear or T = -k * theta - c * w for an angular spring.
	 * Note that if you set mDamping = 0, you will not get an infinite oscillation. Because we integrate physics using an explicit Euler scheme, there is always energy loss.
	 * This is done to keep the simulation from exploding, because with a damping of 0 and even the slightest rounding error, the oscillation could become bigger and bigger until the simulation explodes.
	 */
	UPROPERTY(EditAnywhere)
	float							Damping = 0.0f;
};
