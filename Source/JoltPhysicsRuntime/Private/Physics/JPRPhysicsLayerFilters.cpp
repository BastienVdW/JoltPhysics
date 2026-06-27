// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0

#include "Physics/JPRPhysicsLayerFilters.h"

#include "Physics/JPRPhysicsLayerDataAsset.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace
{
	class FJPRBroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
	{
	public:
		explicit FJPRBroadPhaseLayerInterface(const UJPRPhysicsLayerDataAsset& InPhysicsLayer)
			: PhysicsLayer(&InPhysicsLayer)
		{
		}

		virtual JPH::uint GetNumBroadPhaseLayers() const override { return PhysicsLayer->GetBroadPhaseLayerCount(); }
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer InLayer) const override
		{
			return static_cast<JPH::BroadPhaseLayer>(PhysicsLayer->GetObjectBroadPhaseLayerIndex(static_cast<int32>(InLayer)));
		}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer InLayer) const override
		{
			LayerName = PhysicsLayer->GetBroadPhaseLayerName(static_cast<int32>(InLayer.GetValue())).ToString();
			return TCHAR_TO_ANSI(*LayerName);
		}
#endif

	private:
		TWeakObjectPtr<const UJPRPhysicsLayerDataAsset> PhysicsLayer;
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
		mutable FString LayerName;
#endif
	};

	class FJPRObjectLayerPairFilter final : public JPH::ObjectLayerPairFilter
	{
	public:
		explicit FJPRObjectLayerPairFilter(const UJPRPhysicsLayerDataAsset& InPhysicsLayer) : PhysicsLayer(&InPhysicsLayer) {}
		virtual bool ShouldCollide(JPH::ObjectLayer InLayer1, JPH::ObjectLayer InLayer2) const override
		{
			return PhysicsLayer->CanObjectCollide(static_cast<int32>(InLayer1), static_cast<int32>(InLayer2));
		}

	private:
		TWeakObjectPtr<const UJPRPhysicsLayerDataAsset> PhysicsLayer;
	};

	class FJPRObjectVsBroadPhaseLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter
	{
	public:
		explicit FJPRObjectVsBroadPhaseLayerFilter(const UJPRPhysicsLayerDataAsset& InPhysicsLayer) : PhysicsLayer(&InPhysicsLayer) {}
		virtual bool ShouldCollide(JPH::ObjectLayer InLayer1, JPH::BroadPhaseLayer InLayer2) const override
		{
			return PhysicsLayer->CanObjectCollideWithBroadphase(static_cast<int32>(InLayer1), static_cast<int32>(InLayer2.GetValue()));
		}

	private:
		TWeakObjectPtr<const UJPRPhysicsLayerDataAsset> PhysicsLayer;
	};
}

TSharedPtr<JPH::BroadPhaseLayerInterface> JPR::Private::CreateBroadPhaseLayerInterface(const UJPRPhysicsLayerDataAsset& PhysicsLayer)
{
	return MakeShared<FJPRBroadPhaseLayerInterface>(PhysicsLayer);
}

TSharedPtr<JPH::ObjectLayerPairFilter> JPR::Private::CreateObjectLayerPairFilter(const UJPRPhysicsLayerDataAsset& PhysicsLayer)
{
	return MakeShared<FJPRObjectLayerPairFilter>(PhysicsLayer);
}

TSharedPtr<JPH::ObjectVsBroadPhaseLayerFilter> JPR::Private::CreateObjectVsBroadPhaseLayerFilter(const UJPRPhysicsLayerDataAsset& PhysicsLayer)
{
	return MakeShared<FJPRObjectVsBroadPhaseLayerFilter>(PhysicsLayer);
}
