// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0

#pragma once

#include "Engine/DataAsset.h"

#include "JPRPhysicsLayerDataAsset.generated.h"

USTRUCT()
struct JOLTPHYSICSRUNTIMECORE_API FJPRPhysicsCollisionPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(GetOptions="GetBroadPhaseLayerNames"))
	FName BroadPhaseLayer = NAME_None;

	UPROPERTY(EditAnywhere, EditFixedSize)
	TMap<FName, bool> LayerCollisionPresets;

	UPROPERTY(EditAnywhere, EditFixedSize)
	TMap<FName, bool> BroadPhaseCollisionPresets;
};

UCLASS()
class JOLTPHYSICSRUNTIMECORE_API UJPRPhysicsLayerDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UJPRPhysicsLayerDataAsset();

	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	static int32 GetLayerIndex(const FDataTableRowHandle& Handle, int32 DefaultValue = 0);

	bool ShouldUseLandscapeCollision() const;
	float GetLandscapeFriction() const;
	bool ShouldUseWorldStaticMeshCollision() const;
	float GetWorldStaticMeshFriction() const;
	FName GetObjectLayer(int32 ObjectLayer) const;
	const TArray<FName>& GetObjectLayers() const;
	bool CanObjectCollide(int32 ObjectLayer1, int32 ObjectLayer2) const;
	bool CanObjectCollideWithBroadphase(int32 ObjectLayer1, int32 BroadphaseLayer2) const;
	bool CanObjectCollideWithBroadphase(const FName& ObjectLayer, const FName& BroadphaseLayer) const;
	FName GetObjectBroadPhaseLayer(int32 ObjectLayer) const;
	FName GetObjectBroadPhaseLayer(const FName& ObjectLayerName) const;
	int32 GetObjectBroadPhaseLayerIndex(int32 ObjectLayer, int32 DefaultValue = 0) const;
	FName GetBroadPhaseLayer(int32 BroadPhaseLayer) const;
	int32 GetBroadPhaseLayerCount() const;
	FName GetBroadPhaseLayerName(int32 BroadphaseLayer) const;
	FDataTableRowHandle GetStaticLayerHandle() const;

	UFUNCTION(CallInEditor, Category=Editor)
	void UpdateLayers();

protected:
	UPROPERTY(EditAnywhere)
	bool bUseLandscapeCollision = true;

	UPROPERTY(EditAnywhere, meta=(EditCondition=bUseLandscapeCollision, EditConditionHides))
	float LandscapeFriction = 0.4f;

	UPROPERTY(EditAnywhere)
	bool bUseWorldStaticMeshCollision = true;

	UPROPERTY(EditAnywhere, meta=(EditCondition=bUseWorldStaticMeshCollision, EditConditionHides))
	float WorldStaticMeshFriction = 0.4f;

	UPROPERTY(EditAnywhere, meta=(RowType="/Script/JoltPhysicsRuntimeCore.JPRPhysicsLayerTableRow"))
	TObjectPtr<UDataTable> LayerTable;

	UPROPERTY(EditAnywhere)
	TArray<FName> BroadPhaseLayers;

	UPROPERTY(EditAnywhere, EditFixedSize, meta=(ShowOnlyInnerProperties))
	TMap<FName, FJPRPhysicsCollisionPreset> CollisionPresets;

private:
	UFUNCTION()
	const TArray<FName>& GetBroadPhaseLayerNames() const;

	UPROPERTY(Transient)
	TArray<FName> LayerNames;

	void UpdateBroadphaseLayers();
};
