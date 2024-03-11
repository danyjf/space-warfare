// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
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

ACPP_SimulationGameMode::ACPP_SimulationGameMode()
{
    //FString JsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/SimulationConfig.json");
    FString JsonPath = FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/ISSSimulationConfig.json");
    UJsonReadWrite::ReadStructFromJsonFile<FSimulationConfigStruct>(JsonPath, &SimulationConfig);
    CurrentPlayerNumber = 0;
    StartingCurrency = 300; // Millions
    bWaitingForPlayers = true;
}

void ACPP_SimulationGameMode::BeginPlay()
{
    Super::BeginPlay();

    GravityManager = Cast<ACPP_GravityManager>(GetWorld()->SpawnActor(GravityManagerBlueprint));

    InitializeSimulationVariables();
}

// Called every frame
void ACPP_SimulationGameMode::Tick(float DeltaTime)
{
    if (bWaitingForPlayers)
    {
        TArray<AActor*> PlayerControllers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_CameraOrbitController::StaticClass(), PlayerControllers);

        if (PlayerControllers.Num() < Cast<UCPP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->MaxNumberOfPlayersInSession)
        {
            return;
        }

        for (AActor* Actor : PlayerControllers)
        {
            ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(Actor);
            if (!PlayerController->Ready)
            {
                return;
            }
        }

        TArray<AActor*> Satellites;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
        for (AActor* Actor : Satellites)
        {
            ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
            Satellite->StaticMeshComponent->SetSimulatePhysics(true);
        }

        bWaitingForPlayers = false;
    }
}

// Called at a fixed DeltaTime to update physics
void ACPP_SimulationGameMode::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
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

void ACPP_SimulationGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(NewPlayer);
    CameraOrbitController->PlayerNumber = CurrentPlayerNumber;
    CameraOrbitController->Currency = StartingCurrency;

    // Create a GroundStationManager for each player
    ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(GetWorld()->SpawnActor(GroundStationManagerBlueprint));
    GroundStationManager->SetOwner(CameraOrbitController);
    GroundStationManager->PlayerNumber = CurrentPlayerNumber;

    // Assign the owners of the ground stations
    TArray<AActor*> GroundStations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_GroundStation::StaticClass(), GroundStations);
    for (AActor* Actor : GroundStations)
    {
        ACPP_GroundStation* GroundStation = Cast<ACPP_GroundStation>(Actor);
        if (GroundStation->PlayerNumber == CurrentPlayerNumber)
        {
            GroundStation->SetOwner(CameraOrbitController);
            GroundStationManager->AddGroundStation(GroundStation);
        }
    }

    // Assign the owners of the satellites
    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    for (AActor* Actor : Satellites)
    {
        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
        if (Satellite->PlayerNumber == CurrentPlayerNumber)
        {
            Satellite->SetOwner(CameraOrbitController);
        }
    }

    // Create a SatelliteLauncher for each player
    ACPP_SatelliteLauncher* SatelliteLauncher = Cast<ACPP_SatelliteLauncher>(GetWorld()->SpawnActor(SatelliteLauncherBlueprint));
    SatelliteLauncher->SetOwner(CameraOrbitController);
    SatelliteLauncher->PlayerNumber = CurrentPlayerNumber;
    SatelliteLauncher->Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));

    CurrentPlayerNumber++;
}

void ACPP_SimulationGameMode::InitializeSimulationVariables()
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

    TArray<AActor*> Satellites;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    for (AActor* Actor : Satellites)
    {
        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(Actor);
        Satellite->StaticMeshComponent->SetSimulatePhysics(false);

        for (FSatelliteStruct& SatelliteConfig : SimulationConfig.Satellites)
        {
            if (Satellite->Name != SatelliteConfig.Name)
            {
                continue;
            }

			FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);

            Satellite->SetActorLocation(OrbitalState.Location);
            Satellite->SetActorScale3D(FVector(SatelliteConfig.Size));
            Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
            Satellite->GravityComponent->SetMass(SatelliteConfig.Mass);
            Satellite->GravityComponent->SetGravitationalParameter(GravityManager->GravitationalConstant * SatelliteConfig.Mass);
        }
    }
}
