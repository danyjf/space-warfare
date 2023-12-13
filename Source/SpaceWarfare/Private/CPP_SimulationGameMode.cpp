// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "JsonReadWrite.h"
#include "FileReadWrite.h"

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

	// Calculate gravity forces between planet and all satellites
	for (ACPP_Satellite* Satellite : Satellites)
	{
		FVector GravityForce = UUniverse::CalculateGravityForce(Satellite, Planet);

		Satellite->AddForce(GravityForce);
		Planet->AddForce(-GravityForce);
	}

	// Calculate gravity forces between all satellites
	for (int i = 0; i < Satellites.Num(); i++)
	{
		for (int j = i + 1; j < Satellites.Num(); j++)
		{
			FVector GravityForce = UUniverse::CalculateGravityForce(Satellites[i], Satellites[j], GravitationalConstant);

			Satellites[i]->AddForce(GravityForce);
			Satellites[j]->AddForce(-GravityForce);
		}
	}

	// Apply the forces with semi implicit euler integrator
	UUniverse::SemiImplicitEulerIntegrator(Planet, ScaledDeltaTime);
	for (ACPP_Satellite* Satellite : Satellites)
	{
		UUniverse::SemiImplicitEulerIntegrator(Satellite, ScaledDeltaTime);
	}

	// Calculate current time
	ElapsedTime += DeltaTime * TimeScale;
	FTimespan ElapsedEpoch;
	ElapsedEpoch = ElapsedEpoch.FromSeconds(ElapsedTime);

	CurrentEpoch = InitialEpoch;
	CurrentEpoch += ElapsedEpoch;
}

void ACPP_SimulationGameMode::PrintSimulationData()
{
	const FGeographicCoordinates& GeographicCoordinates = Satellites[0]->GetGeographicCoordinates();
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Current Epoch: %s; Longitude: %f; Latitude: %f; Altitude: %f"),
		*CurrentEpoch.ToString(),
		GeographicCoordinates.Longitude,
		GeographicCoordinates.Latitude,
		GeographicCoordinates.Altitude
	);
	/*const FGeographicCoordinates& GeographicCoordinates = Satellites[0]->GetGeographicCoordinates();
	UFileReadWrite::WriteFile(
		FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/Results.txt"), 
		FString::Printf(
			TEXT("Longitude: %f; Latitude: %f; Altitude: %f\n"), 
			GeographicCoordinates.Longitude, 
			GeographicCoordinates.Latitude, 
			GeographicCoordinates.Altitude
		),
		true
	);*/
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
	//GravitationalConstant = SimulationConfig.GravitationalConstant * SimulationConfig.TimeScale * SimulationConfig.TimeScale;
	GravitationalConstant = SimulationConfig.GravitationalConstant;
	TimeScale = SimulationConfig.TimeScale;

	FDateTime::ParseIso8601(*SimulationConfig.Planet.Epoch, InitialEpoch);

	Planet->Initialize(
		SimulationConfig.Planet.Name, 
		SimulationConfig.Planet.Mass, 
		SimulationConfig.Planet.Size, 
		//SimulationConfig.Planet.GM * TimeScale * TimeScale,
		SimulationConfig.Planet.GM,
		SimulationConfig.Planet.RotationSpeed * TimeScale,
		InitialEpoch
	);

	for (FSatelliteStruct& SatelliteConfig : SimulationConfig.Satellites)
	{
		bool SatelliteExists = false;
		for (ACPP_Satellite* Satellite : Satellites)
		{
			if (Satellite->Name != SatelliteConfig.Name)
			{
				continue;
			}

			SatelliteExists = true;
			FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
			Satellite->Initialize(
				SatelliteConfig.Name, 
				SatelliteConfig.Mass, 
				SatelliteConfig.Size, 
				OrbitalState.Location, 
				//OrbitalState.Velocity * TimeScale
				OrbitalState.Velocity
			);
		}

		if (!SatelliteExists)
		{
			ACPP_Satellite* NewSatellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
			NewSatellite->OrbitingPlanet = Planet;

			FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
			NewSatellite->Initialize(
				SatelliteConfig.Name, 
				SatelliteConfig.Mass, 
				SatelliteConfig.Size, 
				OrbitalState.Location, 
				//OrbitalState.Velocity * TimeScale
				OrbitalState.Velocity
			);

			Satellites.Add(NewSatellite);
		}
	}
}
