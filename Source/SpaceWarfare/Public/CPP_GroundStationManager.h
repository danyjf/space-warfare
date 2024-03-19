// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SatelliteCommands.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewSatelliteDetected, FName, UniqueID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSatelliteDestroyed, FName, UniqueID);

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    TArray<class ACPP_GroundStation*> GroundStations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, ACPP_Satellite*> OverpassingSatellites;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, FSatelliteInfo> FriendlyTrackedSatellites;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, class ACPP_OrbitSpline*> FriendlySatelliteOrbits;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, FSatelliteInfo> EnemyTrackedSatellites;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, class ACPP_OrbitSpline*> EnemySatelliteOrbits;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TMap<FName, class ACPP_OrbitSpline*> AsteroidOrbits;

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
    void ClientNewAsteroidTracked(const FName& UniqueID, const FVector& Location, const FVector& Velocity);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewFriendlySatelliteTracked(const FName& UniqueID, const FSatelliteInfo& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewEnemySatelliteTracked(const FName& UniqueID, const FSatelliteInfo& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientUpdateSatelliteStatus(const FName& UniqueID, const FSatelliteInfo& SatelliteStatus);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientSatelliteDestroyed(const FName& UniqueID);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientAsteroidDestroyed(const FName& UniqueID);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const FTorqueCommand& TorqueCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustCommand(const FThrustCommand& ThrustCommand);

    UFUNCTION(BlueprintCallable)
    void AddGroundStation(class ACPP_GroundStation* GroundStation);

    UFUNCTION(BlueprintCallable)
    void EnableOrbitVisualization(const FName& SatelliteID);

    UFUNCTION(BlueprintCallable)
    void DisableOrbitVisualization(const FName& SatelliteID);

    UFUNCTION(BlueprintCallable)
    const FSatelliteInfo& GetTrackedSatelliteInfo(const FName& SatelliteID);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_Planet* Planet;
    class ACPP_SimulationGameMode* SimulationGameMode;
    bool bInitialized;
    FTimerHandle UpdateSatellitesTimerHandle;
};
