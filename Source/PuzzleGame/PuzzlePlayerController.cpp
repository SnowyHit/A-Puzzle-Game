// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlayerController.h"
#include "PuzzlePiece.h"
#include "Kismet/GameplayStatics.h"

APuzzlePlayerController::APuzzlePlayerController()
{
	bShowMouseCursor = true;        // helpful for UI drag/drop
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}


void APuzzlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	Board = Cast<APuzzleBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleBoard::StaticClass()));
}

void APuzzlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed,  this, &APuzzlePlayerController::OnLMB_Pressed);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &APuzzlePlayerController::OnLMB_Released);
}

bool APuzzlePlayerController::GetMouseOnBoardPlane(FVector& OutWorld) const
{
	FVector WorldOrigin, WorldDirection;
	if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		return false;
	}

	if (FMath::IsNearlyZero(WorldDirection.Z))
	{
		return false;
	}

	// Intersection with the board plane (at BoardPlaneZ)
	const float DistanceAlongRay = (BoardPlaneZ - WorldOrigin.Z) / WorldDirection.Z;
	if (DistanceAlongRay < 0.0f)
	{
		return false;
	}

	OutWorld = WorldOrigin + WorldDirection * DistanceAlongRay;
	return true;
}

void APuzzlePlayerController::OnLMB_Pressed()
{
	GetPawn()->DisableInput(this);
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Visibility, false, hit);
	if (APuzzlePiece* PuzzlePiece = Cast<APuzzlePiece>(hit.GetActor()))
	{
		Dragged = PuzzlePiece;
		// tiny lift so it renders above slot tiles while dragging
		const FVector loc = PuzzlePiece->GetActorLocation();
		PuzzlePiece->SetActorLocation(FVector(loc.X, loc.Y, BoardPlaneZ + DragLift));
	}
}

void APuzzlePlayerController::OnLMB_Released()
{
	GetPawn()->EnableInput(this);
	bLMBPressed = false;
	if (!Dragged || !Board)
	{
		Dragged = nullptr; return;
	}

	FVector dropPoint;
	if (GetMouseOnBoardPlane(dropPoint))
	{
		Board->DropOrSwapAtWorld(Dragged, dropPoint);
	}
	Dragged = nullptr;
}

void APuzzlePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Dragged)
	{
		FVector world;
		if (GetMouseOnBoardPlane(world))
		{
			Dragged->SetActorLocation(world + FVector(0,0,DragLift));
		}
	}
}

