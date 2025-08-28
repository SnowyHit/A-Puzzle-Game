// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzlePiece.generated.h"

struct FPuzzlePieceData;

UCLASS()
class PUZZLEGAME_API APuzzlePiece : public AActor
{
	GENERATED_BODY()

public:
	APuzzlePiece();

	UFUNCTION(BlueprintCallable, Category="Puzzle|Piece")
	void InitializeFromData(const FPuzzlePieceData& Data);

	// Accessors
	UFUNCTION(BlueprintPure, Category="Puzzle|Piece")
	int32 GetDisplayOrder() const { return DisplayOrder; }

	UFUNCTION(BlueprintPure, Category="Puzzle|Piece")
	FString GetPieceID() const { return PieceID; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// Optional editor-assignable overrides
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle|Visual")
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Puzzle|Visual")
	FName TextureParameterName = TEXT("PieceTexture");

	// Data snapshot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle|Data")
	int32 DisplayOrder = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Puzzle|Data")
	FString PieceID;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> MID;
};
