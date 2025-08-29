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

	//Base init to get the id , display order
	UFUNCTION(BlueprintCallable, Category="Puzzle|Piece")
	void InitializeFromData(const FPuzzlePieceData& Data);

	// Saving the data pieces that i would use.
	UFUNCTION(BlueprintPure, Category="Puzzle|Piece")
	int32 GetDisplayOrder() const { return DisplayOrder; }
	UFUNCTION(BlueprintPure, Category="Puzzle|Piece")
	FString GetPieceID() const { return PieceID; }

	//DataPoints for Indexses onboard for easier access to data
	UFUNCTION(BlueprintPure)
	int32 GetCurrentIndex() const { return CurrentIndex; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentIndex(int32 Index) { CurrentIndex = Index; }


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
	UPROPERTY(VisibleAnywhere)
	int32 CurrentIndex = INDEX_NONE;
};
