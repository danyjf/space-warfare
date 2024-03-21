// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_BotGameMode.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GroundStationSpawner.h"
#include "CPP_Planet.h"
#include "JsonReadWrite.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

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
}

void ACPP_BotGameMode::PostLogin(APlayerController* NewPlayer)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(NewPlayer);

    // Create a GroundStationSpawner for each player
    ACPP_GroundStationSpawner* GroundStationSpawner = Cast<ACPP_GroundStationSpawner>(GetWorld()->SpawnActor(GroundStationSpawnerBlueprint));
    GroundStationSpawner->SetOwner(PlayerController);
    GroundStationSpawner->OwnerPlayerID = PlayerController->PlayerID;
    GroundStationSpawner->Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));

    Super::PostLogin(NewPlayer);
}
