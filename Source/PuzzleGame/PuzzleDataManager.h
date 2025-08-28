// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2DDynamic.h"
#include "PuzzleDataManager.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPuzzlePieceData
{
	GENERATED_BODY()

	//General Purpose ID for better handling of data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	int32 DisplayOrder = 0;

	// Filled after image download
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	TObjectPtr<UTexture2DDynamic> Texture = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	FString ImageURL;
};

UCLASS()
class PUZZLEGAME_API APuzzleDataManager : public AActor
{
	GENERATED_BODY()

public:
	APuzzleDataManager();
	// Base API pieces. To make it more flexible.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle|API")
	FString BaseApiUrl = TEXT("http://127.0.0.1:8080");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle|API")
	FString RequestPuzzleJSONPath = TEXT("/RequestImages");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle|API")
	FString SendScorePath = TEXT("/SendScore");

	UPROPERTY(BlueprintReadOnly, Category="Puzzle|API")
	TArray<FPuzzlePieceData> StoredPieceDatas;
	
	// quick getter
	UFUNCTION(BlueprintPure, Category="Puzzle")
	const TArray<FPuzzlePieceData>& GetPieces() const { return StoredPieceDatas; }

	//Called after requesting the json to download the pictures from BP , as its clearer to use on BP
	UFUNCTION(BlueprintCallable, Category="Puzzle")
	void FillPuzzlePieceDataImageByID(const FString& PieceID, UTexture2DDynamic* Texture);

	//Testing the API calls like its a success before making any unneccessary API calls.
	UPROPERTY(EditAnywhere , Category="Puzzle|API")
	bool bTesting;
	//Giving the test its data manually here, Could be expanded with more features on testing
	UPROPERTY(EditAnywhere, Category="Puzzle|API", meta=(MultiLine="true", EditCondition="bTesting"))
	FString CustomJsonData;
	
	/** GET /RequestImages → logs result  -> Saves to StoredPieceData on success*/
	UFUNCTION(BlueprintCallable, Category="Puzzle|API")
	void RequestPuzzleJSON();

	UFUNCTION(BlueprintNativeEvent, Category="Puzzle|API")
	void OnRequestCompletedBP_Event(bool Success = false);
	/** POST /SendScore → logs result -> Updates UI With Response*/
	UFUNCTION(BlueprintCallable, Category="Puzzle|API")
	void SendScore(const FString& Username, float TimeSeconds, int32 MoveCount);
};

