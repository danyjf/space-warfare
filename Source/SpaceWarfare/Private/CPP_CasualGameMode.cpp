// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_CasualGameMode.h"
#include "CPP_Satellite.h"
#include "CPP_PlayerController.h"
#include "CPP_GroundStationManager.h"
#include "CPP_AsteroidSpawner.h"
#include "JsonReadWrite.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

ACPP_CasualGameMode::ACPP_CasualGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when all players have joined the session
void ACPP_CasualGameMode::StartGameplay()
{
    FSimulationConfig SimulationConfig;
    FString SimulationJsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/EarthSimulationConfig.json");
    if (FPaths::FileExists(SimulationJsonPath))
    {
        UJsonReadWrite::ReadStructFromJsonFile<FSimulationConfig>(SimulationJsonPath, &SimulationConfig);
    }
    InitializeSimulation(SimulationConfig);

    for (ACPP_PlayerController* PlayerController : CameraOrbitControllers)
    {
        PlayerController->PlayerStatus = EPlayerStatus::PLACING_SATELLITE_LAUNCHER;
    }

    GetWorld()->GetTimerManager().SetTimer(CheckPlayersReadyTimerHandle, this, &ACPP_CasualGameMode::CheckAllPlayersFinishedPlacingGroundStations, 2.0f, true);
}

void ACPP_CasualGameMode::CheckAllPlayersFinishedPlacingGroundStations()
{
    for (ACPP_PlayerController* PlayerController : CameraOrbitControllers)
    {
        if (!PlayerController->bFinishedPlacingGroundStations)
        {
            return;
        }
    }

    FSatellitesConfig SatellitesConfig;
    FString SatellitesJsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/ISSTestConfig.json");
    if (FPaths::FileExists(SatellitesJsonPath))
    {
        UJsonReadWrite::ReadStructFromJsonFile<FSatellitesConfig>(SatellitesJsonPath, &SatellitesConfig);
    }
    InitializeSatellites(SatellitesConfig.Satellites);

    // TODO: Maybe change this, it is adding all satellites to the clients at the beginning
    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    for (ACPP_GroundStationManager* GroundStationManager : GroundStationManagers)
    {
        for (AActor* SatelliteActor : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(SatelliteActor);
            GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        }
    }

    for (ACPP_PlayerController* PlayerController : CameraOrbitControllers)
    {
        PlayerController->PlayerStatus = EPlayerStatus::GROUND_STATION_CONTROL;
        PlayerController->ClientAllPlayersFinishedPlacingGroundStations();
    }

    ACPP_AsteroidSpawner* AsteroidSpawner = Cast<ACPP_AsteroidSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_AsteroidSpawner::StaticClass()));
    AsteroidSpawner->StartSpawning();

    GetWorld()->GetTimerManager().ClearTimer(CheckPlayersReadyTimerHandle);
}
