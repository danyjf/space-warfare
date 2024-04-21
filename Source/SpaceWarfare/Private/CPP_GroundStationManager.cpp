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

void ACPP_GroundStationManager::UpdateSatelliteInfo()
{
    //TArray<AActor*> Satellites;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    //for (AActor* Actor : Satellites)
    //{
    //    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
    //    ClientUpdateSatelliteInfo(Satellite->GetSatelliteID(), Satellite->GetSatelliteInfo());
    //}

    for (const TPair<int, ACPP_Satellite*>& Elem : OverpassingSatellites)
    {
        ClientUpdateSatelliteInfo(Elem.Key, Elem.Value->GetSatelliteInfo());
    }
}

void ACPP_GroundStationManager::SatelliteEnteredOverpassArea(ACPP_Satellite* Satellite)
{
    FSatelliteInfo SatelliteInfo = Satellite->GetSatelliteInfo();

    if (Satellite->OwnerPlayerID == OwnerPlayerID)
    {
        OverpassingSatellites.Emplace(Satellite->GetSatelliteID(), Satellite);
    }

    if (!TrackedSatellites.Contains(Satellite->GetSatelliteID()))
    {
        ClientNewSatelliteTracked(Satellite->GetSatelliteID(), SatelliteInfo);
    }

    SatelliteCommandManager->SendPendingCommandsToSatellite(Satellite->GetSatelliteID());
}

void ACPP_GroundStationManager::SatelliteExitedOverpassArea(ACPP_Satellite* Satellite)
{
    if (Satellite->OwnerPlayerID == OwnerPlayerID)
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

void ACPP_GroundStationManager::ClientNewSatelliteTracked_Implementation(const int SatelliteID, const FSatelliteInfo& SatelliteInfo)
{
    if (TrackedSatellites.Contains(SatelliteID))
    {
        return;
    }

    TrackedSatellites.Emplace(SatelliteID, SatelliteInfo);
    OnNewSatelliteDetected.Broadcast(SatelliteID, SatelliteInfo);

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
    SatelliteOrbits.Emplace(SatelliteID, OrbitSpline);
}

void ACPP_GroundStationManager::ClientUpdateSatelliteInfo_Implementation(const int SatelliteID, const FSatelliteInfo& SatelliteInfo)
{
    if (!TrackedSatellites.Contains(SatelliteID))
    {
        ClientNewSatelliteTracked(SatelliteID, SatelliteInfo);
        return;
    }

    TrackedSatellites[SatelliteID] = SatelliteInfo;

    FOrbitalState OrbitalState = FOrbitalState(SatelliteInfo.Position, SatelliteInfo.Velocity);
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());

    if (SatelliteOrbits.Contains(SatelliteID) && !SatelliteOrbits[SatelliteID]->IsHidden())
    {
        SatelliteOrbits[SatelliteID]->UpdateOrbit(OrbitalElements, Planet);
    }
}

void ACPP_GroundStationManager::ClientSatelliteDestroyed_Implementation(const int SatelliteID)
{
    OnSatelliteDestroyed.Broadcast(SatelliteID);

    if (TrackedSatellites.Contains(SatelliteID))
    {
        TrackedSatellites.Remove(SatelliteID);
        SatelliteOrbits[SatelliteID]->Destroy();
        SatelliteOrbits.Remove(SatelliteID);
    }
}

void ACPP_GroundStationManager::ClientUpdateSatelliteFuelLevel_Implementation(const int SatelliteID, float FuelPercentage)
{
    OnFuelLevelUpdated.Broadcast(SatelliteID, FuelPercentage);
}

void ACPP_GroundStationManager::EnableOrbitVisualization(const int SatelliteID)
{
    if (SatelliteOrbits.Contains(SatelliteID))
    {
        FOrbitalState OrbitalState = FOrbitalState(TrackedSatellites[SatelliteID].Position, TrackedSatellites[SatelliteID].Velocity);
        FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, Planet->GravityComponent->GetGravitationalParameter());
        SatelliteOrbits[SatelliteID]->UpdateOrbit(OrbitalElements, Planet);

        SatelliteOrbits[SatelliteID]->SetActorHiddenInGame(false);
    }
}

void ACPP_GroundStationManager::DisableOrbitVisualization(const int SatelliteID)
{
    if (SatelliteOrbits.Contains(SatelliteID))
    {
        SatelliteOrbits[SatelliteID]->SetActorHiddenInGame(true);
    }
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
