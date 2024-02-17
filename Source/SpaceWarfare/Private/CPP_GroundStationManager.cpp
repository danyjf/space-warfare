// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"

#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ACPP_GroundStationManager::ACPP_GroundStationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPP_GroundStationManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_GroundStationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus)
{
    if (FriendlyTrackedSatellites.Contains(SatelliteName) || EnemyTrackedSatellites.Contains(SatelliteName))
    {
        return;
    }

    if (SatelliteStatus.PlayerNumber == PlayerNumber)
    {
        FriendlyTrackedSatellites.Emplace(SatelliteName, SatelliteStatus);
        ClientNewFriendlySatelliteTracked(SatelliteName, SatelliteStatus);
    }
    else
    {
        EnemyTrackedSatellites.Emplace(SatelliteName, SatelliteStatus);
        ClientNewEnemySatelliteTracked(SatelliteName, SatelliteStatus);
    }
}

void ACPP_GroundStationManager::ClientNewFriendlySatelliteTracked_Implementation(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus)
{
    FriendlyTrackedSatellites.Emplace(SatelliteName, SatelliteStatus);
    OnNewFriendlySatelliteDetected.Broadcast(SatelliteName);
}

void ACPP_GroundStationManager::ClientNewEnemySatelliteTracked_Implementation(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus)
{
    EnemyTrackedSatellites.Emplace(SatelliteName, SatelliteStatus);
    OnNewEnemySatelliteDetected.Broadcast(SatelliteName);
}

void ACPP_GroundStationManager::AddGroundStation(ACPP_GroundStation* GroundStation)
{
    GroundStations.Add(GroundStation);
    GroundStation->GroundStationManager = this;
}
