// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SatelliteCommands.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewSatelliteDetected, FString, SatelliteName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSatelliteDestroyed, FString, SatelliteName);

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

    UPROPERTY(BlueprintAssignable)
    FSatelliteDestroyed OnSatelliteDestroyed;

    UFUNCTION(BlueprintCallable)
    void SatelliteEnteredOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void SatelliteExitedOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION()
    void UpdateSatelliteStatus();

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewAsteroidTracked(const FString& ObjectName, const FVector& Location, const FVector& Velocity);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewFriendlySatelliteTracked(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewEnemySatelliteTracked(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientUpdateSatelliteStatus(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientSatelliteDestroyed(const FString& SatelliteName);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientAsteroidDestroyed(const FString& ObjectName);

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
    TMap<FString, class ACPP_OrbitSpline*> AsteroidOrbits;
    class ACPP_Planet* Planet;
    class ACPP_SimulationGameMode* SimulationGameMode;
    bool bInitialized;
    FTimerHandle UpdateSatellitesTimerHandle;
};
