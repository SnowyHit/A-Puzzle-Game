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

void APuzzleGameState::AllPiecesAtCorrectSpot_Implementation()
{
	GameOver = true;
}

void APuzzleGameState::PieceDeleted_Implementation(bool CountAsMove)
{
	if (GameOver)
	{
		return;
	}
	if (CountAsMove)
	{
		MoveCount++;
	}
}

void APuzzleGameState::MoveMade_Implementation()
{
	if (GameOver)
	{
		return;
	}
	MoveCount++;
}

void APuzzleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!GameOver)
	{
		ElapsedPuzzleTime += DeltaSeconds;
	}
}
