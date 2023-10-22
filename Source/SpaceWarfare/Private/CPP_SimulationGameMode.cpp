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
		FVector GravityForce = UGravity::CalculateGravityForce(Satellite, Planet, G);

		Satellite->AddForce(GravityForce);
		Planet->AddForce(-GravityForce);
	}

	// Calculate gravity forces between all satellites
	for (int i = 0; i < Satellites.Num(); i++)
	{
		for (int j = i + 1; j < Satellites.Num(); j++)
		{
			FVector GravityForce = UGravity::CalculateGravityForce(Satellites[i], Satellites[j], G);

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
	G = SimulationConfig.GravitationalConstant * SimulationConfig.TimeScale * SimulationConfig.TimeScale;

	Planet->SetMass(SimulationConfig.Planet.Mass);
	Planet->SetSize(SimulationConfig.Planet.Size);
	Planet->SetLocation(FVector(0.0f));
	Planet->SetInitialVelocity(FVector(0.0f));
	Planet->GM = SimulationConfig.Planet.GM * SimulationConfig.TimeScale * SimulationConfig.TimeScale;

	for (ACPP_Satellite* Satellite : Satellites)
	{
		for (FSatelliteStruct& SatelliteConfig : SimulationConfig.Satellites)
		{
			// TODO: check if it is the right satellite

			FOrbitalState OrbitalState = UGravity::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
			Satellite->SetLocation(OrbitalState.Location);
			Satellite->SetInitialVelocity(OrbitalState.Velocity * SimulationConfig.TimeScale);
		}
	}
}
