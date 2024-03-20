// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_BotGameMode.h"
#include "JsonReadWrite.h"

#include "Kismet/KismetSystemLibrary.h"

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
}
