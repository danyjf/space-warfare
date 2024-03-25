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
#include "CPP_GameInstance.h"
#include "JsonReadWrite.h"
#include "Universe.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ACPP_MultiplayerGameMode::ACPP_MultiplayerGameMode()
{
    bAsyncPhysicsTickEnabled = true;

    DefaultNumberOfPlayers = 2;
    CurrentPlayerID = 0;
    StartingCurrency = 300; // Millions
    bWaitingForPlayers = true;
}

void ACPP_MultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();

    GameInstance = Cast<UCPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
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
    if (bWaitingForPlayers)
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

        bWaitingForPlayers = false;
    }
}

// Called at a fixed DeltaTime to update physics
void ACPP_MultiplayerGameMode::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

    if (bWaitingForPlayers)
    {
        return;
    }

	float ScaledDeltaTime = DeltaTime * TimeScale;

	// Calculate current time
	ElapsedTime += ScaledDeltaTime;
	FTimespan ElapsedEpoch;
	ElapsedEpoch = ElapsedEpoch.FromSeconds(ElapsedTime);

	CurrentEpoch = InitialEpoch;
	CurrentEpoch += ElapsedEpoch;
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

    // Create a SatelliteLauncher for each player
    ACPP_SatelliteLauncher* SatelliteLauncher = Cast<ACPP_SatelliteLauncher>(GetWorld()->SpawnActor(SatelliteLauncherBlueprint));
    SatelliteLauncher->SetOwner(PlayerController);
    SatelliteLauncher->OwnerPlayerID = PlayerController->PlayerID;
    SatelliteLauncher->Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

void ACPP_MultiplayerGameMode::InitializeSimulation(const FSimulationConfig& SimulationConfig)
{
	TimeScale = SimulationConfig.TimeScale;
    GravityManager->TimeScale = TimeScale;
    GravityManager->GravitationalConstant = SimulationConfig.GravitationalConstant;

	FDateTime::ParseIso8601(*SimulationConfig.Planet.Epoch, InitialEpoch);

    ACPP_Planet* Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
    Planet->Name = SimulationConfig.Planet.Name;
    Planet->RotationSpeed = FRotator(0, -SimulationConfig.Planet.RotationSpeed * TimeScale, 0);
    Planet->SetRotationAtEpoch(InitialEpoch);
    Planet->SetActorScale3D(FVector(SimulationConfig.Planet.Size));
    Planet->GravityComponent->SetMass(SimulationConfig.Planet.Mass);
    Planet->GravityComponent->SetGravitationalParameter(SimulationConfig.Planet.GM);
}

void ACPP_MultiplayerGameMode::InitializeSatellites(TArray<FSatelliteStruct>& SatellitesConfigs)
{
    ACPP_Planet* Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));

    ShuffleArray(SatellitesConfigs);
    int AssignedPlayerID = 0;
    for (const FSatelliteStruct& SatelliteConfig : SatellitesConfigs)
    {
	    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, Planet->GravityComponent->GetGravitationalParameter());

        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprint));
        Satellite->SetActorLocation(OrbitalState.Location);
        Satellite->SetActorScale3D(FVector(SatelliteConfig.Size));
        Satellite->OrbitingPlanet = Planet;
        Satellite->Label = SatelliteConfig.Name;
        Satellite->OwnerPlayerID = AssignedPlayerID;

        Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
        Satellite->GravityComponent->SetMass(SatelliteConfig.Mass);
        Satellite->GravityComponent->SetGravitationalParameter(GravityManager->GravitationalConstant * SatelliteConfig.Mass);

        AssignedPlayerID = (AssignedPlayerID + 1) % GameInstance->MaxNumberOfPlayersInSession;
    }

    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    TArray<AActor*> PlayerControllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_CameraOrbitController::StaticClass(), PlayerControllers);
    for (AActor* Actor : PlayerControllers)
    {
        ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(Actor);

        // Assign the owners of the satellites
        for (AActor* ActorSatellite : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(ActorSatellite);
            if (Satellite->OwnerPlayerID == PlayerController->PlayerID)
            {
                Satellite->SetOwner(PlayerController);
            }
        }
    }

    // TODO: Maybe change this, it is adding all satellites to the clients at the beginning
    for (ACPP_GroundStationManager* GroundStationManager : GroundStationManagers)
    {
        for (AActor* SatelliteActor : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(SatelliteActor);
            GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        }
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
