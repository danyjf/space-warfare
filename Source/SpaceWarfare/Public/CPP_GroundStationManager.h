// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FString SatelliteName;
};

USTRUCT(BlueprintType)
struct FTorqueCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FVector Torque;
};

USTRUCT(BlueprintType)
struct FThrustCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    bool IsActive;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewSatelliteDetected, FString, SatelliteName);

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    TArray<class ACPP_GroundStation*> GroundStations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int PlayerNumber;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FString, ACPP_Satellite*> OverpassingSatellites;

    UPROPERTY(BlueprintAssignable)
    FNewSatelliteDetected OnNewFriendlySatelliteDetected;

    UPROPERTY(BlueprintAssignable)
    FNewSatelliteDetected OnNewEnemySatelliteDetected;

    UFUNCTION(BlueprintCallable)
    void SatelliteEnteredOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void SatelliteExitedOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewFriendlySatelliteTracked(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewEnemySatelliteTracked(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientUpdateSatelliteStatus(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const FTorqueCommand& TorqueCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustCommand(const FThrustCommand& ThrustCommand);

    UFUNCTION(BlueprintCallable)
    void AddGroundStation(class ACPP_GroundStation* GroundStation);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TMap<FString, FSatelliteStatus> FriendlyTrackedSatellites;
    TMap<FString, class ACPP_OrbitSpline*> FriendlySatelliteOrbits;
    TMap<FString, FSatelliteStatus> EnemyTrackedSatellites;
    TMap<FString, class ACPP_OrbitSpline*> EnemySatelliteOrbits;
    class ACPP_Planet* Planet;
};
