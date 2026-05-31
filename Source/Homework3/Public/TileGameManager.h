// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGameManager.generated.h"

#define MAX_GRID_SIZE 100

class ATileBase;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class HOMEWORK3_API ATileGameManager : public AActor
{
	GENERATED_BODY()

public:
	ATileGameManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void OnActorInteraction(AActor* HitActor, FVector Location, bool bPressed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector GridOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 MapExtendsInGrids;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Tiles")
	TArray<ATileBase*> TileTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	int32 CurrentTileIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	UStaticMeshComponent* GridSelection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	UStaticMeshComponent* SelectedTilePreview;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	bool GridOccupied[MAX_GRID_SIZE][MAX_GRID_SIZE];

	float CurrentYaw;

	FVector LastGridLocation;

	void UpdateSelectedTilePreview(const FVector& GridLocation);
	void CycleTile(int32 Direction);
	void RotateSelectedTile();
	bool ConvertWorldToGridIndex(const FVector& GridLocation, int32& OutGridX, int32& OutGridY) const;
};