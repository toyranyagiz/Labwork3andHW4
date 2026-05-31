// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBase.generated.h"

class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UStaticMesh;
class USceneComponent;

UCLASS()
class HOMEWORK3_API ATileBase : public AActor
{
	GENERATED_BODY()

public:
	ATileBase();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	UStaticMesh* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile")
	UMaterialInterface* BaseMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	UInstancedStaticMeshComponent* InstancedMesh;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Tile")
	USceneComponent* SceneRoot;
};
