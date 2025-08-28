// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBoard.generated.h"

class APuzzlePiece;

UCLASS()
class PUZZLEGAME_API APuzzleBoard : public AActor
{
	 GENERATED_BODY()
public:
    APuzzleBoard();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    int32 Rows = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    int32 Cols = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    float CellSize = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    bool bOriginIsCenter = true;
    
    // Called when a UI-dropped piece should be placed at a world point.
    // If occupied: destroys existing piece, then places the new one.
    UFUNCTION(BlueprintCallable, Category="Grid|Placement")
    bool DropOrReplaceAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint);

    // Helpers
    UFUNCTION(BlueprintPure, Category="Grid")
    bool WorldToCell(const FVector& World, FIntPoint& OutRowCol) const;
    UFUNCTION(BlueprintPure, Category="Grid")
    FVector CellToWorldCenter(const FIntPoint& RowCol) const;
    UFUNCTION(BlueprintPure, Category="Grid")
    int32  CellToIndex(const FIntPoint& RowCol) const { return (RowCol.X>=0 && RowCol.X<Rows && RowCol.Y>=0 && RowCol.Y<Cols) ? RowCol.X*Cols+RowCol.Y : INDEX_NONE; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* Root;

private:
    // SlotIndex → the piece occupying it (or null)
    UPROPERTY(VisibleAnywhere)
    TArray<TWeakObjectPtr<APuzzlePiece>> Slots;

    TArray<int32> SlotInstanceIds;

    void EnsureSlotArrays(); 
    void PlacePieceToIndex(APuzzlePiece* Piece, int32 Index); // snap + occupy + remove visual
    bool ClampCell(FIntPoint& RowCol) const;
};
