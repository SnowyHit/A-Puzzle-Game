// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzleBoard.h"
#include "GameFramework/PlayerController.h"
#include "PuzzlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEGAME_API APuzzlePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APuzzlePlayerController();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly)
	APuzzleBoard* Board;

	//Setting Up Input :: I generally use enhanced input for those , but for this small case i intend to use the base input scheme for less complexity.
	virtual void SetupInputComponent() override;

	//To make drag operation work on pieces
	void OnLMB_Pressed();
	void OnLMB_Released();

	//Getting mouse position on plane when the LMB_Released
	bool GetMouseOnBoardPlane(FVector& OutWorld) const;

private:
	//Making dragging prettier
	UPROPERTY() APuzzlePiece* Dragged = nullptr;

	UPROPERTY(EditAnywhere, Category="Drag") float DragLift = 4.f; // little Z offset while dragging
	float BoardPlaneZ = 0.f;
};
