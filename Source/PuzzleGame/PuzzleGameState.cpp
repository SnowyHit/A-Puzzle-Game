// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleGameState.h"

APuzzleGameState::APuzzleGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APuzzleGameState::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.f;
	MoveCount = 0;
}

void APuzzleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ElapsedTime += DeltaSeconds;
}