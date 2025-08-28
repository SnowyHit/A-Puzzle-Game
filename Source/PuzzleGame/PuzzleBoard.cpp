// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleBoard.h"

#include "PuzzleGameState.h"
#include "PuzzlePiece.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"


APuzzleBoard::APuzzleBoard()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void APuzzleBoard::BeginPlay()
{
    Super::BeginPlay();
    BuildGridVisuals();
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
    if (X < 0.f || Y < 0.f || X >= Width || Y >= Height)
    {
        return false;
    }
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

    const FIntPoint rc(Index/Cols, Index%Cols);
    Piece->SetActorLocation(CellToWorldCenter(rc));

    EnsureSlotArrays();
    // remove any previous occupancy for this piece
    for (int32 i=0;i<Slots.Num();++i)
    {
        if (Slots[i].Get() == Piece) { Slots[i] = nullptr; break; }
    }

    Slots[Index] = Piece;
    Piece->SetCurrentIndex(Index);

    // (optional) visuals:
    SetSlotState(Index, 1.f);
}

bool APuzzleBoard::DropOrReplaceAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint)
{
    if (!DroppedPiece) return false;

    EnsureSlotArrays();

    FIntPoint RowCol;
    if (!WorldToCell(WorldPoint, RowCol))
    {
        // Outside → free old slot (if any) and destroy this piece
        ClearPieceOccupancy(DroppedPiece);
        DroppedPiece->Destroy();
        Cast<APuzzleGameState>(GetWorld()->GetGameState())->PieceDeleted();
        return false;
    }
    const int32 Index = CellToIndex(RowCol);
    if (Index == INDEX_NONE) return false;

    APuzzlePiece* Occupant = Slots[Index].Get();

    if (Occupant && Occupant != DroppedPiece)
    {
        // Slot occupied → destroy the existing piece then replace
        ClearPieceOccupancy(Occupant);
        Occupant->Destroy();
        Slots[Index] = nullptr; // clear before placing the new one
    }

    PlacePieceToIndex(DroppedPiece, Index);
    SetSlotState(Index , true);
    
    Cast<APuzzleGameState>(GetWorld()->GetGameState())->MoveMade();
    return true;
}

bool APuzzleBoard::DropOrSwapAtWorld(APuzzlePiece* DroppedPiece, const FVector& WorldPoint)
{
    if (!DroppedPiece) return false;
    const int32 from = DroppedPiece->GetCurrentIndex();
    EnsureSlotArrays();

    FIntPoint RowCol;
    if (!WorldToCell(WorldPoint, RowCol))
    {
        // Outside → free old slot (if any) and destroy this piece
        ClearPieceOccupancy(DroppedPiece);
        DroppedPiece->Destroy();
        Cast<APuzzleGameState>(GetWorld()->GetGameState())->PieceDeleted(true);
        return false;
    }
    const int32 target = CellToIndex(RowCol);
    if (target == INDEX_NONE) return false;

    // Same slot? just snap back to center
    if (DroppedPiece->GetCurrentIndex() == target)
    {
        PlacePieceToIndex(DroppedPiece, target);
        return true;
    }

    APuzzlePiece* Occupant = Slots[target].Get();

    if (!Occupant)
    {
        // Empty → move in
        SetSlotState(from, false);
        PlacePieceToIndex(DroppedPiece, target);
        Cast<APuzzleGameState>(GetWorld()->GetGameState())->MoveMade();
        return true;
    }

    // Occupied → SWAP
    
    PlacePieceToIndex(Occupant, from);   // occupant to the piece's old slot
    PlacePieceToIndex(DroppedPiece, target);

    Cast<APuzzleGameState>(GetWorld()->GetGameState())->MoveMade();
    return true;
}

void APuzzleBoard::ClearPieceOccupancy(APuzzlePiece* Piece)
{
    if (!Piece) return;
    EnsureSlotArrays();

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].Get() == Piece)
        {
            Slots[i] = nullptr;
            SetSlotState(i, 0.f);   // visual: mark slot empty
            break;
        }
    }
}

TArray<FPuzzlePieceData> APuzzleBoard::GetPiecesNotOnBoard() const
{
    TArray<FPuzzlePieceData> Result;

    // find the first data manager in the world
    APuzzleDataManager* DataManager = Cast<APuzzleDataManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleDataManager::StaticClass())
    );
    if (!DataManager) return Result;

    for (const FPuzzlePieceData& Data : DataManager->StoredPieceDatas)
    {
        bool bFound = false;
        for (const TWeakObjectPtr<APuzzlePiece>& SlotPiece : Slots)
        {
            if (SlotPiece.IsValid() && SlotPiece->GetPieceID() == Data.ID)
            {
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            Result.Add(Data);
        }
    }
    return Result;
}

void APuzzleBoard::BuildGridVisuals()
{
    const int32 Count = Rows * Cols;

    if (!SlotISM)
    {
        SlotISM = NewObject<UInstancedStaticMeshComponent>(this, TEXT("SlotISM"));
        SlotISM->SetupAttachment(GetRootComponent());
        SlotISM->RegisterComponent();
    }

    if (!SlotMesh)
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
        if (Cube.Succeeded()) SlotMesh = Cube.Object;
    }

    SlotISM->ClearInstances();
    SlotISM->SetStaticMesh(SlotMesh);
    if (SlotMaterial) SlotISM->SetMaterial(0, SlotMaterial);

    Slots.SetNum(Count);
    SlotInstanceIds.SetNum(Count);
    SlotISM->SetNumCustomDataFloats(1);
    // Make thin cubes as tiles
    const float ScaleXY   = CellSize / 100.f;
    const float Thickness = 5.f / 100.f;

    for (int32 Row = 0; Row < Rows; ++Row)
    {
        for (int32 Col = 0; Col < Cols; ++Col)
        {
            const int32 SlotIndex = Row * Cols + Col;

            const FVector CenterWS = CellToWorldCenter({ Row, Col }) + FVector(0, 0, -8);

            const FTransform InstanceTransform = FTransform(FQuat::Identity, CenterWS, FVector(ScaleXY - 0.1f, ScaleXY - 0.1f, Thickness));

            const int32 InstanceId = SlotISM->AddInstance(InstanceTransform, true);
            SlotInstanceIds[SlotIndex] = InstanceId;

            // Per-instance custom data: [0]=state, [1]=hover
            SlotISM->SetCustomDataValue(InstanceId, 0, 0.f, false);
        }
    }

    // One flush at the end
    SlotISM->MarkRenderStateDirty();
}

// ---- state ----
void APuzzleBoard::SetSlotState(int32 SlotIndex, float State)
{
    if (!SlotISM || !SlotInstanceIds.IsValidIndex(SlotIndex)) return;
    const int32 instanceId = SlotInstanceIds[SlotIndex];
    if (instanceId < 0) return;
    SlotISM->SetCustomDataValue(instanceId, 0, State, true);  // 0 empty, 1 occupied
}