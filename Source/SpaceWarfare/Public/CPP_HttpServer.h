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

    UPROPERTY()
    int SatelliteID;

    UPROPERTY()
    int OwnerID;

    UPROPERTY()
    FString Label;

    UPROPERTY()
    float Mass;

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector Velocity;

    UPROPERTY()
    FDateTime Epoch;

    UPROPERTY()
    bool IsTargeted;
};

USTRUCT(BlueprintType)
struct FSatelliteListResponse
{
	GENERATED_BODY();

    UPROPERTY()
    int ClientID;

    UPROPERTY()
    int Count;

    UPROPERTY()
    FDateTime RequestTime;

	UPROPERTY()
	TArray<FSatelliteResponse> Satellites;
};

USTRUCT(BlueprintType)
struct FGroundStationResponse
{
    GENERATED_BODY();

    UPROPERTY()
    int OwnerID;

    UPROPERTY()
    FString Label;

    UPROPERTY()
    float FOV;

    UPROPERTY()
    float Altitude;

    UPROPERTY()
    float Latitude;

    UPROPERTY()
    float Longitude;
};

USTRUCT(BlueprintType)
struct FGroundStationListResponse
{
	GENERATED_BODY();

    UPROPERTY()
    int ClientID;

    UPROPERTY()
    int Count;

    UPROPERTY()
    FDateTime RequestTime;

	UPROPERTY()
	TArray<FGroundStationResponse> GroundStations;
};

UCLASS()
class SPACEWARFARE_API ACPP_HttpServer : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
    FString GroundStationListPath = TEXT("/ground-stations");

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString SatelliteListPath = TEXT("/satellites");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString ThrustCommandPath = TEXT("/satellites/:id/thrust");

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
	bool GetGroundStationList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool GetSatelliteList(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool CreateThrustCommand(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	// Just print request for example
	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
