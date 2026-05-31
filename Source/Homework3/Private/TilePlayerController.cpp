// Fill out your copyright notice in the Description page of Project Settings.


#include "TilePlayerController.h"
#include "TileGameManager.h"
#include "InputCoreTypes.h"

void ATilePlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	DefaultMouseCursor = EMouseCursor::Default;
}

void ATilePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GameManager)
	{
		return;
	}

	FHitResult HitResult;
	FVector WorldLocation;
	FVector WorldDirection;

	if (DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = TraceStart + WorldDirection * 100000.0f;

		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;

		if (GetPawn())
		{
			TraceParams.AddIgnoredActor(GetPawn());
		}

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			TraceParams
		);

		if (bHit)
		{
			bool bLeftClickPressedThisFrame = WasInputKeyJustPressed(EKeys::LeftMouseButton);

			GameManager->OnActorInteraction(
				HitResult.GetActor(),
				HitResult.Location,
				bLeftClickPressedThisFrame
			);
		}
	}
}