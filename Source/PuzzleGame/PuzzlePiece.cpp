// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePiece.h"

#include "PuzzleDataManager.h"


// Sets default values
APuzzlePiece::APuzzlePiece()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetGenerateOverlapEvents(true);

	// Try to give a default flat quad
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshObj(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMeshObj.Succeeded())
	{
		Mesh->SetStaticMesh(PlaneMeshObj.Object);
	}
}

void APuzzlePiece::InitializeFromData(const FPuzzlePieceData& Data)
{
	DisplayOrder = Data.DisplayOrder;
	PieceID      = Data.ID;

	// Pick material to apply
	UMaterialInterface* MatToUse = BaseMaterial;
	if (!MatToUse && Mesh && Mesh->GetNumMaterials() > 0)
	{
		MatToUse = Mesh->GetMaterial(0);
	}

	if (MatToUse)
	{
		MID = UMaterialInstanceDynamic::Create(MatToUse, this);
		Mesh->SetMaterial(0, MID);

		// Accept both UTexture2D and UTexture2DDynamic - Download on BP gives up texture2ddynamic 
		if (Data.Texture && MID)
		{
			MID->SetTextureParameterValue(TextureParameterName, Data.Texture);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PuzzlePieceActor] No base material set on %s. Texture parameter cannot be applied."), *GetName());
	}
}

