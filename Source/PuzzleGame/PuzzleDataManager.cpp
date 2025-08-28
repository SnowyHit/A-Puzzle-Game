// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleDataManager.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

class IHttpRequest;

APuzzleDataManager::APuzzleDataManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APuzzleDataManager::FillPuzzlePieceDataImageByID(const FString& PieceID, UTexture2DDynamic* Texture)
{
	if (!Texture) return;

	for (FPuzzlePieceData& Piece : StoredPieceDatas)
	{
		if (Piece.ID == PieceID)
		{
			Piece.Texture = Texture;
			UE_LOG(LogTemp, Log, TEXT("[FillImage] Filled ID=%s with texture %s"), *PieceID, *Texture->GetName());
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[FillImage] No piece found with ID=%s"), *PieceID);
}

void APuzzleDataManager::RequestPuzzleJSON()
{
	//Testing bit
	if (bTesting)
	{
		if (!CustomJsonData.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[RequestImages] Testing enabled → using CustomJsonData string"));
            
			// Parse directly from CustomJsonData
			const FString Body = CustomJsonData;
			TArray<TSharedPtr<FJsonValue>> Root;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
			if (!FJsonSerializer::Deserialize(Reader, Root))
			{
				UE_LOG(LogTemp, Error, TEXT("[RequestImages] CustomJsonData parse FAILED"));
				OnRequestCompletedBP_Event(false);
				return;
			}

			StoredPieceDatas.Empty();
			for (const TSharedPtr<FJsonValue>& V : Root)
			{
				const TSharedPtr<FJsonObject> Obj = V->AsObject();
				if (!Obj) continue;

				FPuzzlePieceData Piece;
				FString OrderStr;
				if (Obj->TryGetStringField(TEXT("display_order"), OrderStr))
				{
					Piece.DisplayOrder = FCString::Atoi(*OrderStr);
				}
				Obj->TryGetStringField(TEXT("image_path"), Piece.ImageURL);
				Piece.ID = FString::Printf(TEXT("Piece_%d"), Piece.DisplayOrder);

				StoredPieceDatas.Add(MoveTemp(Piece));
			}

			UE_LOG(LogTemp, Log, TEXT("[RequestImages] Parsed %d piece(s) from CustomJsonData"), StoredPieceDatas.Num());
			OnRequestCompletedBP_Event(true);
			return;
		}
		
		//If the user didn't give any data
		UE_LOG(LogTemp, Warning, TEXT("[RequestImages] Testing enabled → using mock data (no CustomJsonData)"));

		StoredPieceDatas.Empty();
		for (int32 i = 1; i <= 3; ++i)
		{
			FPuzzlePieceData Piece;
			Piece.ID = FString::Printf(TEXT("Mock_%d"), i);
			Piece.DisplayOrder = i;
			Piece.ImageURL = FString::Printf(TEXT("https://dummy.server/mock_%d.png"), i);
			StoredPieceDatas.Add(Piece);
		}
		OnRequestCompletedBP_Event(true);
		return;
	}

	//Real Request
	const FString Url = BaseApiUrl + RequestPuzzleJSONPath;
    UE_LOG(LogTemp, Log, TEXT("[RequestImages] GET %s"), *Url);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
    Req->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr /*R*/, FHttpResponsePtr Resp, bool bOK)
    {
        if (!bOK || !Resp.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("[RequestImages] FAILED: No response / connection failed"));
        	OnRequestCompletedBP_Event(false);
            return;
        }

        const int32 Code = Resp->GetResponseCode();
        const FString Body = Resp->GetContentAsString();
        UE_LOG(LogTemp, Log,     TEXT("[RequestImages] HTTP %d"), Code);
        UE_LOG(LogTemp, Verbose, TEXT("[RequestImages] Body:\n%s"), *Body);

        // Parse: [{"display_order":"1","image_path":"https://...jpg"}, ...]
        TArray<TSharedPtr<FJsonValue>> Root;
        {
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
            if (!FJsonSerializer::Deserialize(Reader, Root))
            {
                UE_LOG(LogTemp, Error, TEXT("[RequestImages] JSON deserialize failed"));
            	OnRequestCompletedBP_Event(false);
                return;
            }
        }

        TArray<FPuzzlePieceData> Parsed;
        Parsed.Reserve(Root.Num());

        for (const TSharedPtr<FJsonValue>& Item : Root)
        {
            const TSharedPtr<FJsonObject> Obj = Item->AsObject();
            if (!Obj) { continue; }

            FPuzzlePieceData Piece;

            // display_order comes as STRING → convert safely
            FString OrderStr;
            int32 OrderInt = 0;
            if (Obj->TryGetStringField(TEXT("display_order"), OrderStr))
            {
                OrderInt = FCString::Atoi(*OrderStr);
            }
            else
            {
                // fallback: maybe number (if backend changes)
                double OrderDouble = 0.0;
                if (Obj->TryGetNumberField(TEXT("display_order"), OrderDouble))
                {
                    OrderInt = static_cast<int32>(OrderDouble);
                }
            }
            Piece.DisplayOrder = OrderInt;

            // image_path → ImageURL in your struct
            Obj->TryGetStringField(TEXT("image_path"), Piece.ImageURL);

            // generate a simple ID
            Piece.ID = FString::Printf(TEXT("Piece_%d"), Piece.DisplayOrder);

            Parsed.Add(MoveTemp(Piece));
        }

        // Sort by DisplayOrder (ascending)
        Parsed.Sort([](const FPuzzlePieceData& A, const FPuzzlePieceData& B)
        {
            return A.DisplayOrder < B.DisplayOrder;
        });
        StoredPieceDatas = MoveTemp(Parsed);

        UE_LOG(LogTemp, Log, TEXT("[RequestImages] Parsed %d piece(s):"), StoredPieceDatas.Num());
        for (int32 i = 0; i < StoredPieceDatas.Num(); ++i)
        {
            const auto& P = StoredPieceDatas[i];
            UE_LOG(LogTemp, Log, TEXT("  [%02d] ID=%s  Order=%d  URL=%s"),
                i, *P.ID, P.DisplayOrder, *P.ImageURL);
        }
    	OnRequestCompletedBP_Event(true);
    });

    Req->SetURL(Url);
    Req->SetVerb(TEXT("GET"));
    Req->SetHeader(TEXT("Accept"), TEXT("application/json"));
    Req->ProcessRequest();
}

void APuzzleDataManager::OnRequestCompletedBP_Event_Implementation(bool Success)
{
}

void APuzzleDataManager::SendScore(const FString& Username, float TimeSeconds, int32 MoveCount)
{
	const FString Url = BaseApiUrl + SendScorePath;
	UE_LOG(LogTemp, Log, TEXT("[SendScore] POST %s  user=%s  time=%.2f  moves=%d"),
		*Url, *Username, TimeSeconds, MoveCount);

	// Build tiny JSON body
	TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetStringField(TEXT("username"), Username);
	Obj->SetNumberField(TEXT("time"), TimeSeconds);
	Obj->SetNumberField(TEXT("move_count"), MoveCount);

	FString Payload;
	{
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(Obj, Writer);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr /*R*/, FHttpResponsePtr Resp, bool bOK)
	{
		if (!bOK || !Resp.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("[RequestImages] FAILED: No response / connection failed"));
			return;
		}


		UE_LOG(LogTemp, Log, TEXT("[SendScore] HTTP %d"), Resp->GetResponseCode());
		UE_LOG(LogTemp, Verbose, TEXT("[SendScore] Body:\n%s"), *Resp->GetContentAsString());
	});

	Req->SetURL(Url);
	Req->SetVerb(TEXT("POST"));
	Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Req->SetContentAsString(Payload);
	Req->ProcessRequest();
}

