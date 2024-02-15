// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "CPP_GravityManager.h"
#include "CPP_GravityComponent.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GroundStation.h"
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
}

// Called at a fixed DeltaTime to update physics
void ACPP_SimulationGameMode::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

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

    ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(GetWorld()->SpawnActor(GroundStationManagerBlueprint));
    GroundStationManager->SetOwner(CameraOrbitController);

    TArray<AActor*> GroundStations;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_GroundStation::StaticClass(), GroundStations);

    for (AActor* Actor : GroundStations)
    {
        ACPP_GroundStation* GroundStation = Cast<ACPP_GroundStation>(Actor);
        if (GroundStation->PlayerNumber == CurrentPlayerNumber)
        {
            GroundStationManager->AddGroundStation(GroundStation);
        }
    }

    CurrentPlayerNumber++;
}

void ACPP_SimulationGameMode::InitializeSimulationVariables()
{
	TimeScale = SimulationConfig.TimeScale;
    GravityManager->TimeScale = TimeScale;
    GravityManager->GravitationalConstant = SimulationConfig.GravitationalConstant;

	FDateTime::ParseIso8601(*SimulationConfig.Planet.Epoch, InitialEpoch);

    for (UCPP_GravityComponent* GravityComponent : GravityManager->GravityComponents)
    {
        if (ACPP_Planet* Planet = Cast<ACPP_Planet>(GravityComponent->GetOwner()))
        {
            Planet->Name = SimulationConfig.Planet.Name;
            Planet->RotationSpeed = FRotator(0, -SimulationConfig.Planet.RotationSpeed * TimeScale, 0);
            Planet->SetRotationAtEpoch(InitialEpoch);
            Planet->SetActorScale3D(FVector(SimulationConfig.Planet.Size));
            GravityComponent->SetMass(SimulationConfig.Planet.Mass);
            GravityComponent->SetGravitationalParameter(SimulationConfig.Planet.GM);
        }
        else if (ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(GravityComponent->GetOwner()))
        {
            for (FSatelliteStruct& SatelliteConfig : SimulationConfig.Satellites)
            {
			    if (Satellite->Name != SatelliteConfig.Name)
			    {
			    	continue;
			    }

			    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
                Satellite->SetActorLocation(OrbitalState.Location);
                Satellite->SetActorScale3D(FVector(SatelliteConfig.Size));
                GravityComponent->SetVelocity(OrbitalState.Velocity);
                GravityComponent->SetMass(SatelliteConfig.Mass);
                GravityComponent->SetGravitationalParameter(GravityManager->GravitationalConstant * SatelliteConfig.Mass);
            }
        }
    }
}
