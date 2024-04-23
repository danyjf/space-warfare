// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_Satellite.h"
#include "CPP_Thruster.h"
#include "CPP_OrbitSpline.h"
#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_GravityComponent.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_SatelliteCommandManager.h"
#include "CPP_GameState.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACPP_GroundStationManager::ACPP_GroundStationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    SatelliteCommandManager = CreateDefaultSubobject<UCPP_SatelliteCommandManager>(TEXT("SatelliteCommandManager"));
    
    bReplicates = true;
    bInitialized = false;
    OwnerPlayerID = -1;
}

// Called when the game starts or when spawned
void ACPP_GroundStationManager::BeginPlay()
{
	Super::BeginPlay();
	
    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));

    if (HasAuthority())
    {
	    MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
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

    if (!MultiplayerGameMode->GameState->bWaitingForPlayers && !bInitialized)
    {
        bInitialized = true;

        GetWorld()->GetTimerManager().SetTimer(UpdateSatellitesTimerHandle, this, &ACPP_GroundStationManager::UpdateSatelliteInfo, 0.1f, true);
    }
}

void ACPP_GroundStationManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_GroundStationManager, OwnerPlayerID, COND_InitialOnly);
}

/**
 * Send updated info of the satellites overpassing the ground stations 
 * to the clients
*/
void ACPP_GroundStationManager::UpdateSatelliteInfo()
{
    for (const TPair<int, ACPP_Satellite*>& Elem : OverpassingSatellites)
    {
        const ACPP_Satellite* Satellite = Elem.Value;
        ClientUpdateSatelliteInfo(
            Elem.Key, 
            Satellite->GetActorLocation(), 
            Satellite->GetActorRotation(), 
            Satellite->GetVelocity(), 
            MultiplayerGameMode->GetGameState<ACPP_GameState>()->CurrentEpoch
        );
    }
}

/**
 * Called on the server to decide what to do with a satellite that entered
 * the overpassing area of a ground station
*/
void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(ACPP_Satellite* Satellite)
{
    //FSatelliteInfo SatelliteInfo = Satellite->GetSatelliteInfo();

    if (!OverpassingSatellites.Contains(Satellite->GetSatelliteID()))
    {
        OverpassingSatellites.Emplace(Satellite->GetSatelliteID(), Satellite);
    }

    if (!TrackedSatellites.Contains(Satellite->GetSatelliteID()))
    {
        ClientNewSatelliteTracked(
            Satellite->GetSatelliteID(),
            Satellite->OwnerPlayerID,
            Satellite->Label,
            Satellite->StaticMeshComponent->GetMass(),
            Satellite->GetActorLocation(),
            Satellite->GetActorRotation(),
            Satellite->GetVelocity(),
            MultiplayerGameMode->GetGameState<ACPP_GameState>()->CurrentEpoch
        );
    }

    SatelliteCommandManager->SendPendingCommandsToSatellite(Satellite->GetSatelliteID());
}

/**
 * Called on the server to decide what to do with a satellite that left 
 * the overpassing area of a ground station
*/
void ACPP_GroundStationManager::SatelliteExitedOverpassArea(ACPP_Satellite* Satellite)
{
    if (OverpassingSatellites.Contains(Satellite->GetSatelliteID()))
    {
        OverpassingSatellites.Remove(Satellite->GetSatelliteID());
    }
}

void ACPP_GroundStationManager::ClientNewAsteroidTracked_Implementation(const FName& AsteroidID, const FVector& Location, const FVector& Velocity)
{
    ACPP_OrbitSpline* OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    FOrbitalState OrbitalState = FOrbitalState(Location, Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    OrbitSpline->UpdateOrbit(OrbitalElements, Planet);
    OrbitSpline->SetColor(FLinearColor::Yellow);
    AsteroidOrbits.Emplace(AsteroidID, OrbitSpline);
}

void ACPP_GroundStationManager::ClientAsteroidDestroyed_Implementation(const FName& AsteroidID)
{
    AsteroidOrbits[AsteroidID]->Destroy();
    AsteroidOrbits.Remove(AsteroidID);
}

/**
 * Called on the client every time a new satellite is detected by the ground stations
 * to store the information of this satellite on the client
*/
void ACPP_GroundStationManager::ClientNewSatelliteTracked_Implementation(const int SatelliteID, int OwnerID, const FString& Label, float Mass, const FVector& Position, const FRotator& Rotation, const FVector& Velocity, const FDateTime& Epoch)
{
    if (TrackedSatellites.Contains(SatelliteID))
    {
        return;
    }

    FSatelliteInfo SatelliteInfo;
    SatelliteInfo.OwnerID = OwnerID;
    SatelliteInfo.Label = Label;
    SatelliteInfo.Mass = Mass;
    SatelliteInfo.Position = Position;
    SatelliteInfo.Rotation = Rotation;
    SatelliteInfo.Velocity = Velocity;
    SatelliteInfo.Epoch = Epoch;

    TrackedSatellites.Emplace(SatelliteID, SatelliteInfo);
    OnNewSatelliteDetected.Broadcast(SatelliteID, SatelliteInfo);
}

/**
 * Update the position, rotation and velocity of know satellites
*/
void ACPP_GroundStationManager::ClientUpdateSatelliteInfo_Implementation(const int SatelliteID, const FVector& Position, const FRotator& Rotation, const FVector& Velocity, const FDateTime& Epoch)
{
    if (!TrackedSatellites.Contains(SatelliteID))
    {
        return;
    }

    FSatelliteInfo& SatelliteInfo = TrackedSatellites[SatelliteID];
    SatelliteInfo.Position = Position;
    SatelliteInfo.Rotation = Rotation;
    SatelliteInfo.Velocity = Velocity;
    SatelliteInfo.Epoch = Epoch;
}

void ACPP_GroundStationManager::ClientSatelliteDestroyed_Implementation(const int SatelliteID)
{
    OnSatelliteDestroyed.Broadcast(SatelliteID);

    if (TrackedSatellites.Contains(SatelliteID))
    {
        TrackedSatellites.Remove(SatelliteID);
    }
}

void ACPP_GroundStationManager::ClientUpdateSatelliteFuelLevel_Implementation(const int SatelliteID, float FuelPercentage)
{
    OnFuelLevelUpdated.Broadcast(SatelliteID, FuelPercentage);
}

const FSatelliteInfo& ACPP_GroundStationManager::GetTrackedSatelliteInfo(const int SatelliteID)
{
    return TrackedSatellites[SatelliteID];
}

void ACPP_GroundStationManager::PrintOverpassingSatellites()
{
    for (const auto& Elem : OverpassingSatellites)
    {
        UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(OverpassingSatellites.Num()) + " " + Elem.Value->Label);
    }
}
