// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpServerRequest.h"
#include "HttpResultCallback.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_HttpServer.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteResponse
{
	GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    int OwnerID;

    UPROPERTY(BlueprintReadWrite)
    FString Label;

	UPROPERTY(BlueprintReadWrite)
	FVector Position;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;
};

USTRUCT(BlueprintType)
struct FSatelliteListResponse
{
	GENERATED_BODY();

    UPROPERTY()
    int ClientID;

    UPROPERTY()
    FDateTime Epoch;

    UPROPERTY()
    int Count;

	UPROPERTY()
	TArray<FSatelliteResponse> Satellites;
};

UCLASS()
class SPACEWARFARE_API ACPP_HttpServer : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString SatelliteListPath = TEXT("/satellites");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString HttpPathPOST = TEXT("/post-test");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	int32 ServerPort = 8080;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_HttpServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
    class ACPP_GroundStationManager* GroundStationManager;

    void StartServer();
	void StopServer();

	// Callbacks for HttpRequests
	bool GetSatelliteList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool RequestPOST(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	// Just print request for example
	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
