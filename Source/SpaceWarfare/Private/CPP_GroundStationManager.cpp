// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_Satellite.h"
#include "CPP_Thruster.h"
#include "CPP_OrbitSpline.h"
#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_GravityComponent.h"
#include "CPP_SimulationGameMode.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACPP_GroundStationManager::ACPP_GroundStationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    bInitialized = false;
}

// Called when the game starts or when spawned
void ACPP_GroundStationManager::BeginPlay()
{
	Super::BeginPlay();
	
    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));

    if (HasAuthority())
    {
	    SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    }
}

// Called every frame
void ACPP_GroundStationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!HasAuthority())
    {
        return;
    }

    if (!SimulationGameMode->bWaitingForPlayers && !bInitialized)
    {
        bInitialized = true;

        TArray<AActor*> Satellites;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
        for (AActor* Actor : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
            SatelliteEnteredOverpassArea(Satellite);
        }

        GetWorld()->GetTimerManager().SetTimer(UpdateSatellitesTimerHandle, this, &ACPP_GroundStationManager::UpdateSatelliteStatus, 0.1f, true);
    }
}

void ACPP_GroundStationManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_GroundStationManager, OwnerPlayerID, COND_InitialOnly);
}

void ACPP_GroundStationManager::UpdateSatelliteStatus()
{
    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    for (AActor* Actor : Satellites)
    {
        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
        ClientUpdateSatelliteStatus(Satellite->GetFName(), Satellite->GetSatelliteStatus());
    }
}

void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(ACPP_Satellite* Satellite)
{
    FSatelliteInfo SatelliteStatus = Satellite->GetSatelliteStatus();

    if (Satellite->OwnerPlayerID == OwnerPlayerID)
    {
        OverpassingSatellites.Emplace(Satellite->GetFName(), Satellite);
    }

    if (!TrackedSatellites.Contains(Satellite->GetFName()))
    {
        ClientNewSatelliteTracked(Satellite->GetFName(), SatelliteStatus);
    }
}

void ACPP_GroundStationManager::SatelliteExitedOverpassArea(ACPP_Satellite* Satellite)
{
    if (Satellite->OwnerPlayerID == OwnerPlayerID)
    {
        OverpassingSatellites.Remove(Satellite->GetFName());
    }
}

void ACPP_GroundStationManager::ClientNewAsteroidTracked_Implementation(const FName& UniqueID, const FVector& Location, const FVector& Velocity)
{
    ACPP_OrbitSpline* OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    FOrbitalState OrbitalState = FOrbitalState(Location, Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    OrbitSpline->UpdateOrbit(OrbitalElements, Planet);
    OrbitSpline->SetColor(FLinearColor::Yellow);
    AsteroidOrbits.Emplace(UniqueID, OrbitSpline);
}

void ACPP_GroundStationManager::ClientAsteroidDestroyed_Implementation(const FName& UniqueID)
{
    AsteroidOrbits[UniqueID]->Destroy();
    AsteroidOrbits.Remove(UniqueID);
}

void ACPP_GroundStationManager::ClientNewSatelliteTracked_Implementation(const FName& UniqueID, const FSatelliteInfo& SatelliteInfo)
{
    TrackedSatellites.Emplace(UniqueID, SatelliteInfo);
    if (SatelliteInfo.OwnerID == OwnerPlayerID)
    {
        OnNewFriendlySatelliteDetected.Broadcast(UniqueID);
    }
    else
    {
        OnNewEnemySatelliteDetected.Broadcast(UniqueID);
    }

    // Create the orbit spline of the satellite
    ACPP_OrbitSpline* OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    FOrbitalState OrbitalState = FOrbitalState(SatelliteInfo.Position, SatelliteInfo.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    OrbitSpline->UpdateOrbit(OrbitalElements, Planet);
    if (SatelliteInfo.OwnerID == OwnerPlayerID)
    {
        OrbitSpline->SetColor(FLinearColor::Green);
    }
    else
    {
        OrbitSpline->SetColor(FLinearColor::Red);
    }
    SatelliteOrbits.Emplace(UniqueID, OrbitSpline);
}

void ACPP_GroundStationManager::ClientUpdateSatelliteStatus_Implementation(const FName& UniqueID, const FSatelliteInfo& SatelliteStatus)
{
    FOrbitalState OrbitalState = FOrbitalState(SatelliteStatus.Position, SatelliteStatus.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    if (SatelliteOrbits.Contains(UniqueID))
    {
        SatelliteOrbits[UniqueID]->UpdateOrbit(OrbitalElements, Planet);
    }
}

void ACPP_GroundStationManager::ClientSatelliteDestroyed_Implementation(const FName& UniqueID)
{
    OnSatelliteDestroyed.Broadcast(UniqueID);

    if (TrackedSatellites.Contains(UniqueID))
    {
        TrackedSatellites.Remove(UniqueID);
        SatelliteOrbits[UniqueID]->Destroy();
        SatelliteOrbits.Remove(UniqueID);
    }
}

void ACPP_GroundStationManager::ServerSatelliteTorqueCommand_Implementation(const FTorqueCommand& TorqueCommand)
{
    if (!OverpassingSatellites.Contains(TorqueCommand.UniqueID))
    {
        return;
    }

    ACPP_Satellite* Satellite = OverpassingSatellites[TorqueCommand.UniqueID];
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), TorqueCommand.Torque);

    Satellite->StaticMeshComponent->AddTorqueInDegrees(LocalTorque, NAME_None, true);
}

void ACPP_GroundStationManager::ServerSatelliteThrustCommand_Implementation(const FThrustCommand& ThrustCommand)
{
    if (!OverpassingSatellites.Contains(ThrustCommand.UniqueID))
    {
        return;
    }

    ACPP_Satellite* Satellite = OverpassingSatellites[ThrustCommand.UniqueID];

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

void ACPP_GroundStationManager::EnableOrbitVisualization(const FName& SatelliteID)
{
    if (SatelliteOrbits.Contains(SatelliteID))
    {
        SatelliteOrbits[SatelliteID]->SetActorHiddenInGame(false);
    }
}

void ACPP_GroundStationManager::DisableOrbitVisualization(const FName& SatelliteID)
{
    if (SatelliteOrbits.Contains(SatelliteID))
    {
        SatelliteOrbits[SatelliteID]->SetActorHiddenInGame(true);
    }
}

const FSatelliteInfo& ACPP_GroundStationManager::GetTrackedSatelliteInfo(const FName& SatelliteID)
{
    return TrackedSatellites[SatelliteID];
}
