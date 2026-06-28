// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "JPRPhysicsVehicleObject.h"

#include "DrawDebugHelpers.h"
#include "JPRPhysicsVehicleShapeTypes.h"
#include "Kismet/KismetStringLibrary.h"
#include "Physics/JPRPhysicsMath.h"

#if WITH_JOLT_PHYSICS
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;
#endif // WITH_JOLT_PHYSICS

// FJPRPhysicsVehicleBody Begin
struct FJPRVehicleRef
{
	JPH::Ref<VehicleConstraint> Ref;
};

static void CopyCurveFloat(const TObjectPtr<UCurveFloat>& Source, LinearCurve& Target, float Multiplier = 1.0f)
{
	if (Source)
	{
		Target.Clear();
		
		TArray<FRichCurveEditInfo> Curves = Source->GetCurves();
		checkf(Curves.Num() == 1, TEXT("Should always have one curve"));
		FRichCurveEditInfo& Curve = Curves[0];
		for (auto KeyHandleIterator = Curve.CurveToEdit->GetKeyHandleIterator(); KeyHandleIterator; ++KeyHandleIterator)
		{
			const TPair<float, float> TimeValuepair = Curve.CurveToEdit->GetKeyTimeValuePair(*KeyHandleIterator);
			Target.AddPoint(TimeValuepair.Key, TimeValuepair.Value * Multiplier);
		}
	}
}

static void ApplyWheelSettings(const FJPRPhysicsVehicleWheelSettings& WheelSettings, WheelSettingsWV* w, bool bIsFrontWheel = false)
{
	w->mWheelForward = Vec3(1, 0, 0);
	w->mSuspensionMinLength = WheelSettings.SuspensionMinLength * UnrealToJoltPhysicsUnitScale;
	w->mSuspensionMaxLength = WheelSettings.SuspensionMaxLength * UnrealToJoltPhysicsUnitScale;
	w->mSuspensionPreloadLength = WheelSettings.SuspensionPreloadLength * UnrealToJoltPhysicsUnitScale;
	w->mRadius = WheelSettings.Radius * UnrealToJoltPhysicsUnitScale;
	w->mWidth = WheelSettings.Width * UnrealToJoltPhysicsUnitScale;
	w->mEnableSuspensionForcePoint = WheelSettings.EnableSuspensionForcePoint;
	w->mInertia = WheelSettings.Inertia;
	w->mAngularDamping = WheelSettings.AngularDamping;
	w->mMaxSteerAngle = bIsFrontWheel ? DegreesToRadians(WheelSettings.MaxSteerAngle) : 0.0f; // Rear wheel doesn't stear
	w->mMaxBrakeTorque = WheelSettings.MaxBrakeTorque;
	w->mMaxHandBrakeTorque = bIsFrontWheel ? 0.0f : WheelSettings.MaxHandBrakeTorque; // Front wheel doesn't have hand brake
	
	CopyCurveFloat(WheelSettings.LongitudinalFriction, w->mLongitudinalFriction, WheelSettings.LongitudinalFrictionScale);
	CopyCurveFloat(WheelSettings.LateralFriction, w->mLateralFriction, WheelSettings.LateralFrictionScale);
	
	if (WheelSettings.SuspensionSpring.Mode == EJPRPhysicsSpringMode::FrequencyAndDamping)
	{
		w->mSuspensionSpring = SpringSettings(ESpringMode::FrequencyAndDamping, WheelSettings.SuspensionSpring.Frequency, WheelSettings.SuspensionSpring.Damping);
	}
	else if (WheelSettings.SuspensionSpring.Mode == EJPRPhysicsSpringMode::StiffnessAndDamping)
	{
		w->mSuspensionSpring = SpringSettings(ESpringMode::StiffnessAndDamping, WheelSettings.SuspensionSpring.Stiffness, WheelSettings.SuspensionSpring.Damping);
	}
	else
	{
		unimplemented();
	}
}

struct FJPRPhysicsVehicleWheelOffset
{
	float LengthOffset = 0.0f;
	float HeightOffset = 0.0f;
	float WidthOffset = 0.0f;
	bool bIsFrontWheel = false;
};

void FJPRPhysicsVehicleBody::InitVehicle(const FJPRPhysicsVehicleShape& VehicleShape, const FJPRPhysicsBodyParameters& Params, uint32 InBodyID, int32 Layer)
{
#if WITH_JOLT_PHYSICS
	const FVector half_size = UnrealToJoltPhysicsScale(FVector(VehicleShape.Length, VehicleShape.Width, VehicleShape.Height) * 0.5f);
	const float half_vehicle_length = half_size.X;
	const float half_vehicle_width = half_size.Z;
	const float half_vehicle_height = half_size.Y;

	// Create vehicle body
	RefConst<Shape> car_shape = new BoxShape(Vec3(half_vehicle_length, half_vehicle_height, half_vehicle_width));
	BodyCreationSettings body_creation_settings(car_shape, RVec3::sZero(), Quat::sIdentity(),
		static_cast<EMotionType>(Params.MotionType), static_cast<ObjectLayer>(Layer));

	SetupBodyCreationSettings(body_creation_settings, Params);
	
	// Create vehicle constraint
	VehicleConstraintSettings vehicle;

	const TArray<FJPRPhysicsVehicleWheelOffset> WheelOffsets{
		FJPRPhysicsVehicleWheelOffset{ VehicleShape.Wheels.WheelFrontOffset, VehicleShape.Wheels.WheelVerticalOffset, -VehicleShape.Wheels.WheelHorizontalOffset, true },
		FJPRPhysicsVehicleWheelOffset{ VehicleShape.Wheels.WheelFrontOffset, VehicleShape.Wheels.WheelVerticalOffset, VehicleShape.Wheels.WheelHorizontalOffset, true },
		FJPRPhysicsVehicleWheelOffset{ -VehicleShape.Wheels.WheelRearOffset, VehicleShape.Wheels.WheelVerticalOffset, -VehicleShape.Wheels.WheelHorizontalOffset, false },
		FJPRPhysicsVehicleWheelOffset{ -VehicleShape.Wheels.WheelRearOffset, VehicleShape.Wheels.WheelVerticalOffset, VehicleShape.Wheels.WheelHorizontalOffset, false },
	};

	for (const FJPRPhysicsVehicleWheelOffset& WheelOffset : WheelOffsets)
	{
		WheelSettingsWV* w = new WheelSettingsWV;
		w->mPosition = Vec3(WheelOffset.LengthOffset * UnrealToJoltPhysicsUnitScale, WheelOffset.HeightOffset * UnrealToJoltPhysicsUnitScale, -WheelOffset.WidthOffset * UnrealToJoltPhysicsUnitScale);
		ApplyWheelSettings(VehicleShape.Wheels, w, WheelOffset.bIsFrontWheel);
		
		vehicle.mWheels.emplace_back(w);
	}
	
	// Controller
	WheeledVehicleControllerSettings* controller = new WheeledVehicleControllerSettings;
	vehicle.mController = controller;
	vehicle.mMaxPitchRollAngle = DegreesToRadians(VehicleShape.MaxPitchRollAngle);

	controller->mEngine.mMaxTorque = VehicleShape.Engine.MaxTorque;
	controller->mEngine.mMinRPM = VehicleShape.Engine.MinRPM;
	controller->mEngine.mMaxRPM = VehicleShape.Engine.MaxRPM;
	controller->mEngine.mInertia = VehicleShape.Engine.Inertia;
	controller->mEngine.mAngularDamping = VehicleShape.Engine.AngularDamping;

	CopyCurveFloat(VehicleShape.Engine.NormalizedTorque, controller->mEngine.mNormalizedTorque);
	
	auto copy_array = [](const TArray<float>& src)
	{
		return JPH::Array<float>(src.Num(), *src.GetData());
	};
	
	controller->mTransmission.mMode = static_cast<ETransmissionMode>(VehicleShape.Transmission.Mode);
	controller->mTransmission.mGearRatios = copy_array(VehicleShape.Transmission.GearRatios); 
	controller->mTransmission.mReverseGearRatios = copy_array(VehicleShape.Transmission.ReverseGearRatios); 
	controller->mTransmission.mSwitchTime = VehicleShape.Transmission.SwitchTime;
	controller->mTransmission.mClutchReleaseTime = VehicleShape.Transmission.ClutchReleaseTime;
	controller->mTransmission.mSwitchLatency = VehicleShape.Transmission.SwitchLatency;
	controller->mTransmission.mShiftUpRPM = VehicleShape.Transmission.ShiftUpRPM;
	controller->mTransmission.mShiftDownRPM = VehicleShape.Transmission.ShiftDownRPM;
	controller->mTransmission.mClutchStrength = VehicleShape.Transmission.ClutchStrength;

	// Differential
	controller->mDifferentials.resize(VehicleShape.Differentials.Num());

	for (int32 WheelIndex = 0; WheelIndex < VehicleShape.Differentials.Num(); WheelIndex++)
	{
		const FJPRPhysicsVehicleDifferentialSettings& Differential = VehicleShape.Differentials[WheelIndex];
		
		VehicleDifferentialSettings& differential_settings = controller->mDifferentials[WheelIndex];
		differential_settings.mLeftWheel = Differential.LeftWheel;
		differential_settings.mRightWheel = Differential.RightWheel;
		differential_settings.mDifferentialRatio = Differential.DifferentialRatio;
		differential_settings.mLeftRightSplit = Differential.LeftRightSplit;
		differential_settings.mLimitedSlipRatio = Differential.LimitedSlipRatio;
		differential_settings.mEngineTorqueRatio = Differential.EngineTorqueRatio;
	}
	
	Body* car_body = CreateAndSetBody(body_creation_settings, InBodyID);
	
	// Create constraint
	vehicle_constrain = MakeShared<FJPRVehicleRef>();
	vehicle_constrain->Ref = new VehicleConstraint(*car_body, vehicle);
	vehicle_constrain->Ref->SetNumStepsBetweenCollisionTestActive(VehicleShape.NumStepsBetweenCollisionTestActive);
	vehicle_constrain->Ref->SetNumStepsBetweenCollisionTestInactive(VehicleShape.NumStepsBetweenCollisionTestInactive);
	
	// Set the collision tester
	VehicleCollisionTester *tester = new VehicleCollisionTesterRay(static_cast<ObjectLayer>(Layer));
	vehicle_constrain->Ref->SetVehicleCollisionTester(tester);
	
	// Add the vehicle
	GetPhysicsSystem().AddConstraint(vehicle_constrain->Ref);
	GetPhysicsSystem().AddStepListener(vehicle_constrain->Ref);

	// SetupBodyCreationSettings(body_creation_settings, Params);
	
	bTriggerHitEvents = Params.bTriggerHitEvents;
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsVehicleBody::ReleasePhysicsObject()
{
	Desactivate();
	
#if WITH_JOLT_PHYSICS
	if (vehicle_constrain.IsValid())
	{
		GetPhysicsSystem().RemoveStepListener(vehicle_constrain->Ref);
		GetPhysicsSystem().RemoveConstraint(vehicle_constrain->Ref);
		
		vehicle_constrain.Reset();
	}
	
	if (body_id.IsValid())
	{
		GetBodyInterface().DestroyBody(*body_id.Get());

		body_id.Reset();
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsVehicleBody::DrawDebugShape(const UWorld* World, const FColor& Color) const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#if WITH_JOLT_PHYSICS
	if (body_id.IsValid())
	{
		if (!vehicle_constrain.IsValid())
		{
			return;
		}

		const ShapeRefC ShapeRefC = GetBodyInterface().GetShape(*body_id.Get());
		const BoxShape* Shape = static_cast<const BoxShape*>(ShapeRefC.GetPtr());

		const Vec3 HalfExtents = Shape->GetHalfExtent();
		const FVector WorldExtents = JoltPhysicsToUnrealScale(FVector(HalfExtents.GetX(), HalfExtents.GetY(), HalfExtents.GetZ()));

		FVector WorldPosition = FVector::ZeroVector;
		FQuat WorldRotation = FQuat::Identity;
		GetPositionAndRotation(WorldPosition, WorldRotation);

		DrawDebugBox(World, WorldPosition, WorldExtents, WorldRotation, Color);
		
		if (GetBodyInterface().GetMotionType(*body_id.Get()) != EMotionType::Static)
		{
			FString DebugString;
			const FVector Velocity = GetLinearVelocityPerSecond();
			DebugString += FString::Printf(TEXT("Vel: %s\n"), *Velocity.ToString());
			
			DrawDebugString(World, WorldPosition, DebugString);
		}
		
		// Draw our wheels (this needs to be done in the pre update since we draw the bodies too in the state before the step)
		for (int32 w = 0; w < 4; ++w)
		{
			Wheel* wheel = vehicle_constrain->Ref->GetWheels()[w];
			const WheelSettings *settings = wheel->GetSettings();
			RMat44 wheel_transform = vehicle_constrain->Ref->GetWheelWorldTransform(w, Vec3::sAxisY(), Vec3::sAxisX()); // The cylinder we draw is aligned with Y so we specify that as rotational axis
			Vec3 position = wheel_transform.GetTranslation();
			Quat rotation = wheel_transform.GetRotation().GetQuaternion();
			
			const FVector WheelLocation = JoltPhysicsToUnreal(FVector(position.GetX(), position.GetY(), position.GetZ()));			
			const FQuat WheelRotation = JoltPhysicsToUnreal(FQuat(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW()));

			const float Width = settings->mWidth * JoltPhysicsToUnrealUnitScale;
			const float Radius = settings->mRadius * JoltPhysicsToUnrealUnitScale;
			
			const FVector Start = WheelLocation - WheelRotation.GetUpVector() * Width * 0.5f;
			const FVector End = WheelLocation + WheelRotation.GetUpVector() * Width * 0.5f;
			
			DrawDebugCylinder(World, Start, End, Radius, 32, Color);
		}
	}
#endif // WITH_JOLT_PHYSICS
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void FJPRPhysicsVehicleBody::DrawDebugInfo(const UWorld* World, const FColor& Color) const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	FString DebugVehicleInfo;

#if WITH_JOLT_PHYSICS
	if (!vehicle_constrain.IsValid())
	{
		return;
	}
	
	if (const WheeledVehicleController* controller = static_cast<WheeledVehicleController *>(vehicle_constrain->Ref->GetController()))
	{
		const int32 SpeedKilometersPerHour = FMath::RoundToInt(GetSpeedKilometersPerHour(World).Size());
		
		DebugVehicleInfo += FString::Printf(TEXT("Speed: %d km/h\n"), SpeedKilometersPerHour);
		DebugVehicleInfo += FString::Printf(TEXT("Current RPM: %d rpm\n"), static_cast<int32>(controller->GetEngine().GetCurrentRPM()));
		DebugVehicleInfo += FString::Printf(TEXT("Current Gear: %d (bIsSwitchingGear: %s)\n"),
			controller->GetTransmission().GetCurrentGear(),
			*UKismetStringLibrary::Conv_BoolToString(controller->GetTransmission().IsSwitchingGear()));
		DebugVehicleInfo += FString::Printf(TEXT("Current Ratio: %.2f\n"), controller->GetTransmission().GetCurrentRatio());
		DebugVehicleInfo += FString::Printf(TEXT("Clutch Friction: %.2f\n"), controller->GetTransmission().GetClutchFriction());
	}
#endif // WITH_JOLT_PHYSICS
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, DebugVehicleInfo);
	}
#endif // UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
}

void FJPRPhysicsVehicleBody::SetDriverInput(float Forward, float Right, float Brake, float HandBrake)
{	
#if WITH_JOLT_PHYSICS
	if (vehicle_constrain.IsValid())
	{
		WheeledVehicleController *controller = static_cast<WheeledVehicleController *>(vehicle_constrain->Ref->GetController());
		if (controller != nullptr)
		{
			controller->SetDriverInput(Forward, -Right, Brake, HandBrake);
		}
	}
#endif // WITH_JOLT_PHYSICS
}

void FJPRPhysicsVehicleBody::GetDriverInput(float& OutForward, float& OutRight, float& OutBrake, float& OutHandBrake) const
{
#if WITH_JOLT_PHYSICS
	if (vehicle_constrain.IsValid())
	{
		WheeledVehicleController *controller = static_cast<WheeledVehicleController *>(vehicle_constrain->Ref->GetController());
		if (controller != nullptr)
		{
			OutForward = controller->GetRightInput();
			OutRight = -controller->GetRightInput();
			OutBrake = controller->GetBrakeInput();
			OutHandBrake = controller->GetHandBrakeInput();
		}
	}
#endif // WITH_JOLT_PHYSICS
}

FVector FJPRPhysicsVehicleBody::GetSpeedKilometersPerHour(const UWorld* World) const
{
	return GetLinearVelocityPerSecond() * 0.036;
}

TArray<FJPRPhysicsVehicleWheelContact> FJPRPhysicsVehicleBody::GetWheelContacts() const
{
	TArray<FJPRPhysicsVehicleWheelContact> Contacts;
	
#if WITH_JOLT_PHYSICS
	if (vehicle_constrain.IsValid())
	{
		const Wheels& wheels = vehicle_constrain->Ref->GetWheels();
		const int32 wheel_count = wheels.size();
		
		Contacts.SetNum(wheel_count);
		
		for (int32 w = 0; w < wheel_count; ++w)
		{
			const Wheel* wheel = wheels[w];
			if (wheel != nullptr && wheel->HasContact())
			{
				const RVec3 contact_point = wheel->GetContactPosition();
				
				FJPRPhysicsVehicleWheelContact& Contact = Contacts[w];
				Contact.bHasContact = true;
				Contact.Position = JoltPhysicsToUnreal(FVector(contact_point.GetX(), contact_point.GetY(), contact_point.GetZ()));
			}
		}
	}
#endif // WITH_JOLT_PHYSICS
	
	return Contacts;
}

bool FJPRPhysicsVehicleBody::HasRearWheelsContact() const
{
#if WITH_JOLT_PHYSICS
	if (vehicle_constrain.IsValid())
	{
		const Wheels& wheels = vehicle_constrain->Ref->GetWheels();
		const int32 wheel_count = wheels.size();

		// Skip front wheels.
		for (int32 w = 2; w < wheel_count; ++w)
		{
			const Wheel* wheel = wheels[w];
			if (wheel != nullptr && wheel->HasContact())
			{
				return true;
			}
		}
	}
#endif // WITH_JOLT_PHYSICS	
	return false;
}
// FJPRPhysicsHeightFieldBody End
