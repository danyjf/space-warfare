// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_MultiplayerGameMode.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "CPP_GravityManager.h"
#include "CPP_GravityComponent.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
#include "CPP_SatelliteLauncher.h"
#include "CPP_SatelliteLauncherSpawner.h"
#include "CPP_GameInstance.h"
#include "CPP_GameState.h"
#include "JsonReadWrite.h"
#include "Universe.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ACPP_MultiplayerGameMode::ACPP_MultiplayerGameMode()
{
    DefaultNumberOfPlayers = 2;
    CurrentPlayerID = 0;
    CurrentSatelliteID = 0;
    StartingCurrency = 300; // Millions
}

void ACPP_MultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();

    GameInstance = Cast<UCPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    GameState = GetGameState<ACPP_GameState>();
    GravityManager = Cast<ACPP_GravityManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GravityManager::StaticClass()));

    if (GameInstance->MaxNumberOfPlayersInSession == 0)
    {
        GameInstance->MaxNumberOfPlayersInSession = DefaultNumberOfPlayers;
    }
}

// Called when all players have joined the session
void ACPP_MultiplayerGameMode::StartGameplay()
{
}

// Called every frame
void ACPP_MultiplayerGameMode::Tick(float DeltaTime)
{
    if (GameState->bWaitingForPlayers)
    {
        if (CameraOrbitControllers.Num() < GameInstance->MaxNumberOfPlayersInSession)
        {
            return;
        }

        for (ACPP_CameraOrbitController* PlayerController : CameraOrbitControllers)
        {
            if (!PlayerController->bHasNecessaryReplicatedVariables)
            {
                return;
            }
        }

        StartGameplay();

        GameState->bWaitingForPlayers = false;
    }
}

void ACPP_MultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(NewPlayer);
    PlayerController->PlayerID = CurrentPlayerID;
    CurrentPlayerID++;
    PlayerController->Currency = StartingCurrency;
    CameraOrbitControllers.Add(PlayerController);

    // Create a GroundStationManager for each player
    ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(GetWorld()->SpawnActor(GroundStationManagerBlueprint));
    GroundStationManager->SetOwner(PlayerController);
    GroundStationManager->OwnerPlayerID = PlayerController->PlayerID;
    GroundStationManagers.Add(GroundStationManager);

    // Create a SatelliteLauncherSpawner for each player
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = PlayerController;
    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_SatelliteLauncherSpawner* SatelliteLauncherSpawner = Cast<ACPP_SatelliteLauncherSpawner>(GetWorld()->SpawnActor(SatelliteLauncherSpawnerBlueprint, &SpawnLocation, SpawnParameters));
    SatelliteLauncherSpawner->OwnerPlayerID = PlayerController->PlayerID;
}

void ACPP_MultiplayerGameMode::InitializeSimulation(const FSimulationConfig& SimulationConfig)
{
	GameState->TimeScale = SimulationConfig.TimeScale;
    GravityManager->GravitationalConstant = SimulationConfig.GravitationalConstant;

	FDateTime::ParseIso8601(*SimulationConfig.Planet.Epoch, GameState->CurrentEpoch);

    ACPP_Planet* Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
    Planet->Name = SimulationConfig.Planet.Name;
    Planet->RotationSpeed = FRotator(0, -SimulationConfig.Planet.RotationSpeed * GameState->TimeScale, 0);
    Planet->SetRotationAtEpoch(GameState->CurrentEpoch);
    Planet->SetActorScale3D(FVector(SimulationConfig.Planet.Size));
    Planet->GravityComponent->SetMass(SimulationConfig.Planet.Mass);
    Planet->GravityComponent->SetGravitationalParameter(SimulationConfig.Planet.GM);
}

void ACPP_MultiplayerGameMode::InitializeSatellites(TArray<FSatelliteStruct>& SatellitesConfigs)
{
    ACPP_Planet* Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
    TArray<AActor*> PlayerControllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_CameraOrbitController::StaticClass(), PlayerControllers);

    ShuffleArray(SatellitesConfigs);
    int AssignedPlayerID = 0;   // PlayerID that will be assigned to each satellite
    for (const FSatelliteStruct& SatelliteConfig : SatellitesConfigs)
    {
        // Find the PlayerController that will own this satellite
        ACPP_CameraOrbitController* OwnerPlayerController = Cast<ACPP_CameraOrbitController>(PlayerControllers[0]);
        for (AActor* Actor : PlayerControllers)
        {
            OwnerPlayerController = Cast<ACPP_CameraOrbitController>(Actor);
            if (OwnerPlayerController->PlayerID == AssignedPlayerID)
            {
                break;
            }
        }

	    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, Planet->GravityComponent->GetGravitationalParameter());
        FTransform SpawnTransform(FRotator(0.0f, 0.0f, 0.0f), OrbitalState.Location, FVector(SatelliteConfig.Size));
        ACPP_Satellite* Satellite = GetWorld()->SpawnActorDeferred<ACPP_Satellite>(SatelliteBlueprint, SpawnTransform, OwnerPlayerController);
        Satellite->OrbitingPlanet = Planet;
        Satellite->Label = SatelliteConfig.Name;
        Satellite->OwnerPlayerID = AssignedPlayerID;
        Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
        Satellite->GravityComponent->SetMass(SatelliteConfig.Mass);
        Satellite->GravityComponent->SetGravitationalParameter(GravityManager->GravitationalConstant * SatelliteConfig.Mass);
        Satellite->FinishSpawning(SpawnTransform);

        AssignedPlayerID = (AssignedPlayerID + 1) % GameInstance->MaxNumberOfPlayersInSession;
    }
}

template <class T>
void ACPP_MultiplayerGameMode::ShuffleArray(T& InArray)
{
    const int LastIndex = InArray.Num() - 1;
    for (int i = 0; i < LastIndex; i++)
    {
        int Index = FMath::RandRange(0, LastIndex);
        if (i != Index)
        {
            InArray.Swap(i, Index);
        }
    }
}
