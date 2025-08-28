// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlayerController.h"
#include "Components/InstancedStaticMeshComponent.h"
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

void APuzzlePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

