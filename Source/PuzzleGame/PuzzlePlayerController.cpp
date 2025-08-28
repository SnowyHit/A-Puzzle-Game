// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlayerController.h"

APuzzlePlayerController::APuzzlePlayerController()
{
	bShowMouseCursor = true;        // helpful for UI drag/drop
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

