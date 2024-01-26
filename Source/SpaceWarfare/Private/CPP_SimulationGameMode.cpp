// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "CPP_GravityManager.h"
#include "CPP_GravityComponent.h"
#include "JsonReadWrite.h"
#include "FileReadWrite.h"
#include "Universe.h"

#include "JsonObjectConverter.h"	// JsonUtilities module
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


ACPP_SimulationGameMode::ACPP_SimulationGameMode()
{
	//SimulationConfig = ReadSimulationConfigJson("SimulationConfig.json");
	SimulationConfig = ReadSimulationConfigJson("ISSSimulationConfig.json");
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

FSimulationConfigStruct ACPP_SimulationGameMode::ReadSimulationConfigJson(const FString& SimulationConfigPath)
{
	TSharedPtr<FJsonObject> JsonObject = UJsonReadWrite::ReadJson(
		FPaths::Combine(
			FPaths::ProjectContentDir(), 
			"SpaceWarfare/Data/", 
			SimulationConfigPath
		)
	);

	FSimulationConfigStruct Config;
	if (!FJsonObjectConverter::JsonObjectToUStruct<FSimulationConfigStruct >(JsonObject.ToSharedRef(), &Config))
	{
		UKismetSystemLibrary::PrintString(this, "Read struct json failed - Was not able to convert the json object to the desired structure. Is it the right format / struct?");
	}

	return Config;
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
