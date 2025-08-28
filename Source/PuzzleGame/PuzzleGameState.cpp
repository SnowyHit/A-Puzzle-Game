// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleGameState.h"

APuzzleGameState::APuzzleGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APuzzleGameState::BeginPlay()
{
	Super::BeginPlay();
	ElapsedPuzzleTime = 0.f;
	MoveCount = 0;
}

void APuzzleGameState::PieceDeleted_Implementation(bool CountAsMove)
{
	if (CountAsMove)
	{
		MoveCount++;
	}
}

void APuzzleGameState::MoveMade_Implementation()
{
	MoveCount++;
}

void APuzzleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ElapsedPuzzleTime += DeltaSeconds;
}
