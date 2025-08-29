// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PuzzleGameState.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEGAME_API APuzzleGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	APuzzleGameState();
	
	UPROPERTY(BlueprintReadOnly, Category="Puzzle|Stats")
	int32 MoveCount = 0;

	UPROPERTY(BlueprintReadOnly, Category="Puzzle|Stats")
	float ElapsedPuzzleTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Puzzle|Stats")
	bool GameOver = false;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void MoveMade();
	UFUNCTION(BlueprintNativeEvent)
	void PieceDeleted(bool CountAsMove = false);
	UFUNCTION(BlueprintNativeEvent)
	void AllPiecesAtCorrectSpot();
};
