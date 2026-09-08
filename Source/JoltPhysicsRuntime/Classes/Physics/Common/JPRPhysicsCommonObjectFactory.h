// Copyright (C) 2024 Van de Walle Bastien
// SPDX-License-Identifier: MIT

#pragma once

#include "Physics/JPRPhysicsObjectFactory.h"

#include "JPRPhysicsCommonObjectFactory.generated.h"

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsBoxObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsSphereObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsCapsuleObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsMeshObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer,
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsConvexHullObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer, 
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsHeightFieldObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer, 
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};

UCLASS()
class JOLTPHYSICSRUNTIME_API UJPRPhysicsStaticCompoundObjectFactory : public UJPRPhysicsObjectFactory
{
	GENERATED_BODY()

public:
	virtual TSharedPtr<FJPRPhysicsBody> BuildPhysicsObject(UObject* Outer, 
		uint32 BodyID, const FInstancedStruct& Shape, const FJPRPhysicsBodyParameters& Params) const override;
};
