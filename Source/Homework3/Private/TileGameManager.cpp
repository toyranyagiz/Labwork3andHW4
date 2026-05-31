// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGameManager.h"
#include "TileBase.h"
#include "TilePlayerController.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "InputCoreTypes.h"

ATileGameManager::ATileGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

	GridSize = 100;
	GridOffset = FVector(0.0f, 0.0f, 2.0f);
	MapExtendsInGrids = 25;

	CurrentTileIndex = 0;
	CurrentYaw = 0.0f;
	LastGridLocation = FVector::ZeroVector;

	for (int32 X = 0; X < MAX_GRID_SIZE; X++)
	{
		for (int32 Y = 0; Y < MAX_GRID_SIZE; Y++)
		{
			GridOccupied[X][Y] = false;
		}
	}

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	GridSelection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridSelection"));
	GridSelection->SetupAttachment(SceneRoot);
	GridSelection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GridSelection->SetMobility(EComponentMobility::Movable);

	SelectedTilePreview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectedTilePreview"));
	SelectedTilePreview->SetupAttachment(SceneRoot);
	SelectedTilePreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SelectedTilePreview->SetMobility(EComponentMobility::Movable);
	SelectedTilePreview->SetVisibility(false);
	SelectedTilePreview->CastShadow = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));

	if (PlaneMesh.Succeeded())
	{
		GridSelection->SetStaticMesh(PlaneMesh.Object);
	}
}

void ATileGameManager::BeginPlay()
{
	Super::BeginPlay();

	ATilePlayerController* PlayerController = Cast<ATilePlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0)
	);

	if (PlayerController)
	{
		PlayerController->GameManager = this;
	}
}

void ATileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileGameManager::OnActorInteraction(AActor* HitActor, FVector Location, bool bPressed)
{
	if (!HitActor)
	{
		return;
	}

	if (TileTypes.Num() == 0)
	{
		return;
	}

	FVector GridLocation;
	GridLocation.X = FMath::GridSnap(Location.X, static_cast<float>(GridSize));
	GridLocation.Y = FMath::GridSnap(Location.Y, static_cast<float>(GridSize));
	GridLocation.Z = 0.0f;

	LastGridLocation = GridLocation;

	if (GridSelection)
	{
		GridSelection->SetWorldLocation(GridLocation + GridOffset);
		GridSelection->SetWorldRotation(FRotator::ZeroRotator);

		float SelectionScale = GridSize / 100.0f;
		GridSelection->SetWorldScale3D(FVector(SelectionScale, SelectionScale, 1.0f));
	}

	UpdateSelectedTilePreview(GridLocation);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PlayerController)
	{
		return;
	}

	if (PlayerController->WasInputKeyJustPressed(EKeys::MouseScrollDown))
	{
		CycleTile(1);
		UpdateSelectedTilePreview(GridLocation);
		return;
	}

	if (PlayerController->WasInputKeyJustPressed(EKeys::MouseScrollUp))
	{
		CycleTile(-1);
		UpdateSelectedTilePreview(GridLocation);
		return;
	}

	if (PlayerController->WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		RotateSelectedTile();
		UpdateSelectedTilePreview(GridLocation);
		return;
	}

	if (bPressed)
	{
		int32 GridX;
		int32 GridY;

		if (!ConvertWorldToGridIndex(GridLocation, GridX, GridY))
		{
			UE_LOG(LogTemp, Warning, TEXT("Clicked outside the map."));
			return;
		}

		if (GridOccupied[GridX][GridY])
		{
			UE_LOG(LogTemp, Warning, TEXT("This grid cell is already occupied."));
			return;
		}

		if (!TileTypes.IsValidIndex(CurrentTileIndex))
		{
			return;
		}

		ATileBase* SelectedTile = TileTypes[CurrentTileIndex];

		if (!SelectedTile || !SelectedTile->InstancedMesh || !SelectedTile->BaseMesh)
		{
			return;
		}

		FRotator TileRotation = FRotator(0.0f, CurrentYaw, 0.0f);

		FVector TileScale = SelectedTile->PlacementScale;
		FVector TileOffset = TileRotation.RotateVector(SelectedTile->PlacementOffset);

		FTransform TileTransform(
			TileRotation,
			GridLocation + TileOffset,
			TileScale
		);

		SelectedTile->InstancedMesh->AddInstance(TileTransform, true);

		GridOccupied[GridX][GridY] = true;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Tile placed at X: %f, Y: %f, Z: %f | Tile Index: %d | Rotation Yaw: %f"),
			GridLocation.X,
			GridLocation.Y,
			GridLocation.Z,
			CurrentTileIndex,
			CurrentYaw
		);
	}
}

void ATileGameManager::UpdateSelectedTilePreview(const FVector& GridLocation)
{
	if (!SelectedTilePreview)
	{
		return;
	}

	if (TileTypes.Num() == 0)
	{
		SelectedTilePreview->SetVisibility(false);
		return;
	}

	if (!TileTypes.IsValidIndex(CurrentTileIndex))
	{
		CurrentTileIndex = 0;
	}

	ATileBase* SelectedTile = TileTypes[CurrentTileIndex];

	if (!SelectedTile || !SelectedTile->BaseMesh)
	{
		SelectedTilePreview->SetVisibility(false);
		return;
	}

	SelectedTilePreview->SetStaticMesh(SelectedTile->BaseMesh);

	if (SelectedTile->BaseMaterial)
	{
		SelectedTilePreview->SetMaterial(0, SelectedTile->BaseMaterial);
	}

	FRotator TileRotation = FRotator(0.0f, CurrentYaw, 0.0f);

	FVector TileScale = SelectedTile->PlacementScale;
	FVector TileOffset = TileRotation.RotateVector(SelectedTile->PlacementOffset);

	FVector PreviewLocation = GridLocation + GridOffset + TileOffset + FVector(0.0f, 0.0f, 5.0f);

	SelectedTilePreview->SetWorldLocation(PreviewLocation);
	SelectedTilePreview->SetWorldRotation(TileRotation);
	SelectedTilePreview->SetWorldScale3D(TileScale);
	SelectedTilePreview->SetVisibility(true);
}

void ATileGameManager::CycleTile(int32 Direction)
{
	if (TileTypes.Num() == 0)
	{
		return;
	}

	CurrentTileIndex += Direction;

	if (CurrentTileIndex >= TileTypes.Num())
	{
		CurrentTileIndex = 0;
	}

	if (CurrentTileIndex < 0)
	{
		CurrentTileIndex = TileTypes.Num() - 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Tile Index: %d"), CurrentTileIndex);
}

void ATileGameManager::RotateSelectedTile()
{
	CurrentYaw += 90.0f;

	if (CurrentYaw >= 360.0f)
	{
		CurrentYaw = 0.0f;
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Rotation Yaw: %f"), CurrentYaw);
}

bool ATileGameManager::ConvertWorldToGridIndex(const FVector& GridLocation, int32& OutGridX, int32& OutGridY) const
{
	OutGridX = FMath::RoundToInt(GridLocation.X / GridSize) + MapExtendsInGrids;
	OutGridY = FMath::RoundToInt(GridLocation.Y / GridSize) + MapExtendsInGrids;

	int32 MaxAllowedIndex = MapExtendsInGrids * 2;

	if (OutGridX < 0 || OutGridY < 0)
	{
		return false;
	}

	if (OutGridX > MaxAllowedIndex || OutGridY > MaxAllowedIndex)
	{
		return false;
	}

	if (OutGridX >= MAX_GRID_SIZE || OutGridY >= MAX_GRID_SIZE)
	{
		return false;
	}

	return true;
}
