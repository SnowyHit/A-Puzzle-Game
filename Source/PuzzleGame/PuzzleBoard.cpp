// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleBoard.h"

#include "PuzzlePiece.h"


APuzzleBoard::APuzzleBoard()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APuzzleBoard::EnsureSlotArrays()
{
    const int32 Count = Rows * Cols;
    if (Slots.Num() != Count)  Slots.SetNum(Count);
    if (SlotInstanceIds.Num() != Count) { SlotInstanceIds.SetNum(Count); for (int32& Id : SlotInstanceIds) Id = -1; }
}

bool APuzzleBoard::ClampCell(FIntPoint& RowCol) const
{
    bool inside = true;
    if (RowCol.X < 0) { RowCol.X=0; inside=false; }
    if (RowCol.Y < 0) { RowCol.Y=0; inside=false; }
    if (RowCol.X >= Rows) { RowCol.X=Rows-1; inside=false; }
    if (RowCol.Y >= Cols) { RowCol.Y=Cols-1; inside=false; }
    return inside;
}

bool APuzzleBoard::WorldToCell(const FVector& World, FIntPoint& OutRowCol) const
{
    const FTransform T = GetActorTransform();
    const FVector Local = T.InverseTransformPosition(World);

    const float Width = Cols*CellSize;
    const float Height = Rows*CellSize;

    float X = Local.X;
    float Y = Local.Y;
    if (bOriginIsCenter) { X += Width*0.5f; Y += Height*0.5f; }

    FIntPoint RowCol(FMath::FloorToInt(Y/CellSize), FMath::FloorToInt(X/CellSize));
    ClampCell(RowCol);
    OutRowCol = RowCol;
    return true;
}

FVector APuzzleBoard::CellToWorldCenter(const FIntPoint& RowCol) const
{
    const float Width = Cols*CellSize;
    const float Height = Rows*CellSize;

    float X = (RowCol.Y + 0.5f) * CellSize;
    float Y = (RowCol.X + 0.5f) * CellSize;
    if (bOriginIsCenter) { X -= Width*0.5f; Y -= Height*0.5f; }

    const FVector Local(X, Y, 0.f);
    return GetActorTransform().TransformPosition(Local);
}
void APuzzleBoard::PlacePieceToIndex(APuzzlePiece* Piece, int32 Index)
{
    if (!Piece || Index == INDEX_NONE) return;

    // move actor to center
    FIntPoint RowCol(Index/Cols, Index%Cols);
    Piece->SetActorLocation(CellToWorldCenter(RowCol));

    // track occupancy
    EnsureSlotArrays();

    // remove any previous occupancy for this piece
    for (int32 i=0;i<Slots.Num();++i)
    {
        if (Slots[i].Get() == Piece) { Slots[i] = nullptr; break; }
    }

    // place new occupancy
    Slots[Index] = Piece;
}

bool APuzzleBoard::DropOrReplaceAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint)
{
    if (!DroppedPiece) return false;

    EnsureSlotArrays();

    FIntPoint RowCol; WorldToCell(WorldPoint, RowCol);
    const int32 Index = CellToIndex(RowCol);
    if (Index == INDEX_NONE) return false;

    APuzzlePiece* Occupant = Slots[Index].Get();

    if (Occupant && Occupant != DroppedPiece)
    {
        // Slot occupied → destroy the existing piece then replace
        Occupant->Destroy();
        Slots[Index] = nullptr; // clear before placing the new one
    }

    PlacePieceToIndex(DroppedPiece, Index);
    return true;
}