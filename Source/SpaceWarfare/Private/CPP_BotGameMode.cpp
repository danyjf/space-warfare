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

    TArray<AActor*> PlayerControllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_CameraOrbitController::StaticClass(), PlayerControllers);
    for (AActor* Actor : PlayerControllers)
    {
        ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(Actor);
        PlayerController->PlayerStatus = EPlayerStatus::PLACING_GROUND_STATIONS;
    }

    GetWorld()->GetTimerManager().SetTimer(CheckPlayersReadyTimerHandle, this, &ACPP_BotGameMode::CheckAllPlayersFinishedPlacingGroundStations, 2.0f, true);
}

void ACPP_BotGameMode::PostLogin(APlayerController* NewPlayer)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(NewPlayer);

    // Create a GroundStationSpawner for each player
    ACPP_GroundStationSpawner* GroundStationSpawner = Cast<ACPP_GroundStationSpawner>(GetWorld()->SpawnActor(GroundStationSpawnerBlueprint));
    GroundStationSpawner->SetOwner(PlayerController);
    GroundStationSpawner->OwnerPlayerID = PlayerController->PlayerID;

    Super::PostLogin(NewPlayer);
}

void ACPP_BotGameMode::CheckAllPlayersFinishedPlacingGroundStations()
{
    TArray<AActor*> PlayerControllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_CameraOrbitController::StaticClass(), PlayerControllers);
    for (AActor* Actor : PlayerControllers)
    {
        ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(Actor);
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

    // TODO: Refactor everything after this point, maybe put some stuff inside the InitializeSatellites function
    for (AActor* Actor : PlayerControllers)
    {
        ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(Actor);
        PlayerController->PlayerStatus = EPlayerStatus::GROUND_STATION_CONTROL;

        // Assign the owners of the satellites
        TArray<AActor*> Satellites;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
        for (AActor* ActorSatellite : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(ActorSatellite);
            if (Satellite->OwnerPlayerID == PlayerController->PlayerID)
            {
                Satellite->SetOwner(PlayerController);
            }
        }

        PlayerController->ClientAllPlayersFinishedPlacingGroundStations();
    }

    TArray<AActor*> GroundStationManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass(), GroundStationManagers);
    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    for (AActor* Actor : GroundStationManagers)
    {
        ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(Actor);
        for (AActor* ActorSatellite : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(ActorSatellite);
            GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        }
    }

    GetWorld()->GetTimerManager().ClearTimer(CheckPlayersReadyTimerHandle);
}
