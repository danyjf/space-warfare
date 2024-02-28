// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_Satellite.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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

void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(ACPP_Satellite* Satellite)
{
    if (Satellite->PlayerNumber == PlayerNumber)
    {
        OverpassingSatellites.Emplace(Satellite->Name, Satellite);

        if (!FriendlyTrackedSatellites.Contains(Satellite->Name))
        {
            FriendlyTrackedSatellites.Emplace(Satellite->Name, Satellite->GetSatelliteStatus());
            ClientNewFriendlySatelliteTracked(Satellite->Name, Satellite->GetSatelliteStatus());
        }
    }
    else
    {
        if (!EnemyTrackedSatellites.Contains(Satellite->Name))
        {
            EnemyTrackedSatellites.Emplace(Satellite->Name, Satellite->GetSatelliteStatus());
            ClientNewEnemySatelliteTracked(Satellite->Name, Satellite->GetSatelliteStatus());
        }
    }
}

void ACPP_GroundStationManager::SatelliteExitedOverpassArea(ACPP_Satellite* Satellite)
{
    if (Satellite->PlayerNumber == PlayerNumber)
    {
        OverpassingSatellites.Remove(Satellite->Name);
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

void ACPP_GroundStationManager::ServerSatelliteTorqueCommand_Implementation(const FTorqueCommand& TorqueCommand)
{
    if (!OverpassingSatellites.Contains(TorqueCommand.SatelliteName))
    {
        return;
    }

    ACPP_Satellite* Satellite = OverpassingSatellites[TorqueCommand.SatelliteName];
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), TorqueCommand.Torque);

    UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Satellite->FindComponentByClass(UStaticMeshComponent::StaticClass()));
    StaticMeshComponent->AddTorqueInDegrees(LocalTorque, FName(NAME_None), true);
}

void ACPP_GroundStationManager::AddGroundStation(ACPP_GroundStation* GroundStation)
{
    GroundStations.Add(GroundStation);
    GroundStation->GroundStationManager = this;
}
