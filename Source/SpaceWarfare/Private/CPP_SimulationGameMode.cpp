// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "JsonReadWrite.h"

#include "JsonObjectConverter.h"	// JsonUtilities module
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetSystemLibrary.h"


ACPP_SimulationGameMode::ACPP_SimulationGameMode()
{
	//SimulationConfig = ReadSimulationConfigJson("SimulationConfig.json");
	SimulationConfig = ReadSimulationConfigJson("ISSSimulationConfig.json");
}

// Called at a fixed DeltaTime to update physics
void ACPP_SimulationGameMode::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

	// Calculate gravity forces between planet and all satellites
	for (ACPP_Satellite* Satellite : Satellites)
	{
		FVector GravityForce = UGravity::CalculateGravityForce(Satellite, Planet);

		Satellite->AddForce(GravityForce);
		Planet->AddForce(-GravityForce);
	}

	// Calculate gravity forces between all satellites
	for (int i = 0; i < Satellites.Num(); i++)
	{
		for (int j = i + 1; j < Satellites.Num(); j++)
		{
			FVector GravityForce = UGravity::CalculateGravityForce(Satellites[i], Satellites[j], GravitationalConstant);

			Satellites[i]->AddForce(GravityForce);
			Satellites[j]->AddForce(-GravityForce);
		}
	}

	// Apply the forces with semi implicit euler integrator
	UGravity::SemiImplicitEulerIntegrator(Planet, DeltaTime);
	for (ACPP_Satellite* Satellite : Satellites)
	{
		UGravity::SemiImplicitEulerIntegrator(Satellite, DeltaTime);
	}
}

FSimulationConfigStruct ACPP_SimulationGameMode::ReadSimulationConfigJson(const FString& SimulationConfigPath)
{
	bool bSuccess;
	FString InfoMessage;
	TSharedPtr<FJsonObject> JsonObject = UJsonReadWrite::ReadJson(
		FPaths::Combine(
			FPaths::ProjectContentDir(), 
			"SpaceWarfare/Data/", 
			SimulationConfigPath
		), 
		bSuccess, 
		InfoMessage
	);

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *FString(InfoMessage));
	}

	FSimulationConfigStruct Config;
	if (!FJsonObjectConverter::JsonObjectToUStruct<FSimulationConfigStruct >(JsonObject.ToSharedRef(), &Config))
	{
		UKismetSystemLibrary::PrintString(this, "Read struct json failed - Was not able to convert the json object to the desired structure. Is it the right format / struct?");
	}

	return Config;
}

void ACPP_SimulationGameMode::InitializeSimulationVariables()
{
	GravitationalConstant = SimulationConfig.GravitationalConstant * SimulationConfig.TimeScale * SimulationConfig.TimeScale;

	Planet->Initialize(
		SimulationConfig.Planet.Name, 
		SimulationConfig.Planet.Mass, 
		SimulationConfig.Planet.Size, 
		SimulationConfig.Planet.GM * SimulationConfig.TimeScale * SimulationConfig.TimeScale,
		SimulationConfig.Planet.RotationSpeed * SimulationConfig.TimeScale,
		SimulationConfig.Planet.Epoch
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
			FOrbitalState OrbitalState = UGravity::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
			Satellite->Initialize(
				SatelliteConfig.Name, 
				SatelliteConfig.Mass, 
				SatelliteConfig.Size, 
				OrbitalState.Location, 
				OrbitalState.Velocity * SimulationConfig.TimeScale
			);
		}

		if (!SatelliteExists)
		{
			ACPP_Satellite* NewSatellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
			NewSatellite->OrbitingPlanet = Planet;

			FOrbitalState OrbitalState = UGravity::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
			NewSatellite->Initialize(
				SatelliteConfig.Name, 
				SatelliteConfig.Mass, 
				SatelliteConfig.Size, 
				OrbitalState.Location, 
				OrbitalState.Velocity * SimulationConfig.TimeScale
			);

			Satellites.Add(NewSatellite);
		}
	}
}
