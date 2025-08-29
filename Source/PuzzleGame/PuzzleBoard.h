// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzleDataManager.h"
#include "GameFramework/Actor.h"
#include "PuzzleBoard.generated.h"

class APuzzlePiece;

UCLASS()
class PUZZLEGAME_API APuzzleBoard : public AActor
{
	 GENERATED_BODY()
public:
    APuzzleBoard();
    virtual void BeginPlay() override;

	//For data and setting board attiributes. Setting Up Slots.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    int32 Rows = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    int32 Cols = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    float CellSize = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    bool bOriginIsCenter = true;
	
	UPROPERTY()
    APuzzleDataManager* DataManager;

     // Called when a UI-dropped piece should be placed at a world point.
    // If occupied when dropped from ui : destroys existing piece, then places the new one.
    UFUNCTION(BlueprintCallable, Category="Grid|Placement")
    bool DropOrReplaceAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint);

	//This is the 3D world swapping-Dropping mechanic
    bool DropOrSwapAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint);

	//Check all pieces to ensure they are on correct spot , must be called after move made for performance issues.
    void CheckBoardCompletion();
    
    void ClearPieceOccupancy(APuzzlePiece* Piece);
    //Create Visuals for the places players can put puzzle pieces
    void BuildGridVisuals();

	//This is for visuals , setting the material values on ISM to change the mat values based on slot. 
    void SetSlotState(int32 SlotIndex, float State);
	
     // Helpers for snapping / finding the cells
    UFUNCTION(BlueprintPure, Category="Grid")
    bool WorldToCell(const FVector& World, FIntPoint& OutRowCol) const;
    UFUNCTION(BlueprintPure, Category="Grid")
    FVector CellToWorldCenter(const FIntPoint& RowCol) const;
    UFUNCTION(BlueprintPure, Category="Grid")
    int32  CellToIndex(const FIntPoint& RowCol) const { return (RowCol.X>=0 && RowCol.X<Rows && RowCol.Y>=0 && RowCol.Y<Cols) ? RowCol.X*Cols+RowCol.Y : INDEX_NONE; }
    //
    UPROPERTY(VisibleAnywhere, Category="Components")
    UInstancedStaticMeshComponent* SlotISM = nullptr;
    
    UPROPERTY(VisibleAnywhere)
    TArray<TWeakObjectPtr<APuzzlePiece>> Slots;

	//Helper Function for UI 
    UFUNCTION(BlueprintCallable, Category="Puzzle|Board")
    TArray<FPuzzlePieceData> GetPiecesNotOnBoard() const;
    
    TArray<int32> SlotInstanceIds;
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USceneComponent* Root;

private:
    
    UPROPERTY(EditAnywhere, Category="Grid|Visual")
    UStaticMesh* SlotMesh = nullptr;             // default: Cube

    UPROPERTY(EditAnywhere, Category="Grid|Visual")
    UMaterialInterface* SlotMaterial = nullptr;

	//Ensuring the created slots / visuals allign so there's less chance to a bug happening.
    void EnsureSlotArrays();
    void PlacePieceToIndex(APuzzlePiece* Piece, int32 Index); // snap + occupy
    //Clamps the indexes / Also returns if given rowcol is inside the boundaries
    bool ClampCell(FIntPoint& RowCol) const;
};
