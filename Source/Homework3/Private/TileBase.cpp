// Fill out your copyright notice in the Description page of Project Settings.


#include "TileBase.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInterface.h"

ATileBase::ATileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetupAttachment(SceneRoot);

	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedMesh->SetMobility(EComponentMobility::Movable);
}

void ATileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (InstancedMesh)
	{
		if (BaseMesh)
		{
			InstancedMesh->SetStaticMesh(BaseMesh);
		}

		if (BaseMaterial)
		{
			InstancedMesh->SetMaterial(0, BaseMaterial);
		}
	}
}

void ATileBase::BeginPlay()
{
	Super::BeginPlay();

	if (InstancedMesh)
	{
		InstancedMesh->ClearInstances();
	}
}

void ATileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

