// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_Satellite.h"
#include "CPP_Thruster.h"
#include "CPP_OrbitSpline.h"
#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_GravityComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


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
	
    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

// Called every frame
void ACPP_GroundStationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(ACPP_Satellite* Satellite)
{
    FSatelliteStatus SatelliteStatus = Satellite->GetSatelliteStatus();

    if (Satellite->PlayerNumber == PlayerNumber)
    {
        OverpassingSatellites.Emplace(Satellite->Name, Satellite);

        if (!FriendlyTrackedSatellites.Contains(Satellite->Name))
        {
            FriendlyTrackedSatellites.Emplace(Satellite->Name, SatelliteStatus);
            ClientNewFriendlySatelliteTracked(Satellite->Name, SatelliteStatus);
        }
    }
    else
    {
        if (!EnemyTrackedSatellites.Contains(Satellite->Name))
        {
            EnemyTrackedSatellites.Emplace(Satellite->Name, SatelliteStatus);
            ClientNewEnemySatelliteTracked(Satellite->Name, SatelliteStatus);
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

    // Create the orbit spline of the satellite
    if (!OrbitSplineBlueprint)
    {
        return;
    }

    ACPP_OrbitSpline* OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    FOrbitalState OrbitalState = FOrbitalState(SatelliteStatus.Position, SatelliteStatus.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    OrbitSpline->UpdateOrbit(OrbitalElements, Planet);
    OrbitSpline->SetColor(FLinearColor::Green);
    FriendlySatelliteOrbits.Emplace(SatelliteName, OrbitSpline);
}

void ACPP_GroundStationManager::ClientNewEnemySatelliteTracked_Implementation(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus)
{
    EnemyTrackedSatellites.Emplace(SatelliteName, SatelliteStatus);
    OnNewEnemySatelliteDetected.Broadcast(SatelliteName);

    // Create the orbit spline of the satellite
    if (!OrbitSplineBlueprint)
    {
        return;
    }

    ACPP_OrbitSpline* OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    FOrbitalState OrbitalState = FOrbitalState(SatelliteStatus.Position, SatelliteStatus.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    OrbitSpline->UpdateOrbit(OrbitalElements, Planet);
    OrbitSpline->SetColor(FLinearColor::Red);
    EnemySatelliteOrbits.Emplace(SatelliteName, OrbitSpline);
}

void ACPP_GroundStationManager::ClientUpdateSatelliteStatus_Implementation(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus)
{
    FOrbitalState OrbitalState = FOrbitalState(SatelliteStatus.Position, SatelliteStatus.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    if (FriendlySatelliteOrbits.Contains(SatelliteName))
    {
        FriendlySatelliteOrbits[SatelliteName]->UpdateOrbit(OrbitalElements, Planet);
    }
    else if (EnemySatelliteOrbits.Contains(SatelliteName))
    {
        EnemySatelliteOrbits[SatelliteName]->UpdateOrbit(OrbitalElements, Planet);
    }
}

void ACPP_GroundStationManager::ClientSatelliteDestroyed_Implementation(const FString& SatelliteName)
{
    OnSatelliteDestroyed.Broadcast(SatelliteName);

    if (FriendlyTrackedSatellites.Contains(SatelliteName))
    {
        FriendlyTrackedSatellites.Remove(SatelliteName);
        FriendlySatelliteOrbits[SatelliteName]->Destroy();
        FriendlySatelliteOrbits.Remove(SatelliteName);
    }
    else if (EnemyTrackedSatellites.Contains(SatelliteName))
    {
        EnemyTrackedSatellites.Remove(SatelliteName);
        EnemySatelliteOrbits[SatelliteName]->Destroy();
        EnemySatelliteOrbits.Remove(SatelliteName);
    }
}

void ACPP_GroundStationManager::ServerSatelliteTorqueCommand_Implementation(const FTorqueCommand& TorqueCommand)
{
    if (!OverpassingSatellites.Contains(TorqueCommand.SatelliteName))
    {
        return;
    }

    ACPP_Satellite* Satellite = OverpassingSatellites[TorqueCommand.SatelliteName];
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), TorqueCommand.Torque);

    Satellite->StaticMeshComponent->AddTorqueInDegrees(LocalTorque, FName(NAME_None), true);
}

void ACPP_GroundStationManager::ServerSatelliteThrustCommand_Implementation(const FThrustCommand& ThrustCommand)
{
    if (!OverpassingSatellites.Contains(ThrustCommand.SatelliteName))
    {
        return;
    }

    ACPP_Satellite* Satellite = OverpassingSatellites[ThrustCommand.SatelliteName];

    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));
    if (ThrustCommand.IsActive)
    {
        Thruster->ActivateThruster();
    }
    else
    {
        Thruster->DeactivateThruster();
    }
}

void ACPP_GroundStationManager::AddGroundStation(ACPP_GroundStation* GroundStation)
{
    GroundStations.Add(GroundStation);
    GroundStation->GroundStationManager = this;
}
