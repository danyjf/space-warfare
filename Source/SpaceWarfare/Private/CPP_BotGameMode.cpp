// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_BotGameMode.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GroundStationSpawner.h"
#include "CPP_GroundStationManager.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "JsonReadWrite.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

ACPP_BotGameMode::ACPP_BotGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when all players have joined the session
void ACPP_BotGameMode::StartGameplay()
{
    FSimulationConfig SimulationConfig;
    FString SimulationJsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/EarthSimulationConfig.json");
    if (FPaths::FileExists(SimulationJsonPath))
    {
        UJsonReadWrite::ReadStructFromJsonFile<FSimulationConfig>(SimulationJsonPath, &SimulationConfig);
    }
    InitializeSimulation(SimulationConfig);

    for (ACPP_CameraOrbitController* PlayerController : CameraOrbitControllers)
    {
        PlayerController->PlayerStatus = EPlayerStatus::PLACING_GROUND_STATIONS;
    }

    GetWorld()->GetTimerManager().SetTimer(CheckPlayersReadyTimerHandle, this, &ACPP_BotGameMode::CheckAllPlayersFinishedPlacingGroundStations, 2.0f, true);
}

void ACPP_BotGameMode::PostLogin(APlayerController* NewPlayer)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(NewPlayer);

    // Create a GroundStationSpawner for each player
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = PlayerController;
    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_GroundStationSpawner* GroundStationSpawner = Cast<ACPP_GroundStationSpawner>(GetWorld()->SpawnActor(GroundStationSpawnerBlueprint, &SpawnLocation, SpawnParameters));
    GroundStationSpawner->OwnerPlayerID = PlayerController->PlayerID;

    Super::PostLogin(NewPlayer);
}

void ACPP_BotGameMode::CheckAllPlayersFinishedPlacingGroundStations()
{
    for (ACPP_CameraOrbitController* PlayerController : CameraOrbitControllers)
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
            GroundStationManager->SatelliteExitedOverpassArea(Satellite);
        }
    }

    for (ACPP_CameraOrbitController* PlayerController : CameraOrbitControllers)
    {
        PlayerController->PlayerStatus = EPlayerStatus::GROUND_STATION_CONTROL;
        PlayerController->ClientAllPlayersFinishedPlacingGroundStations();
    }

    GetWorld()->GetTimerManager().ClearTimer(CheckPlayersReadyTimerHandle);
}
