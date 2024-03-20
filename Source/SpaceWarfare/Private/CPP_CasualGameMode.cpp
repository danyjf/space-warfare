// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_CasualGameMode.h"
#include "CPP_Satellite.h"
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

    FSatellitesConfig SatellitesConfig;
    FString SatellitesJsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/ISSTestConfig.json");
    if (FPaths::FileExists(SatellitesJsonPath))
    {
        UJsonReadWrite::ReadStructFromJsonFile<FSatellitesConfig>(SatellitesJsonPath, &SatellitesConfig);
    }
    InitializeSatellites(SatellitesConfig.Satellites);
}
