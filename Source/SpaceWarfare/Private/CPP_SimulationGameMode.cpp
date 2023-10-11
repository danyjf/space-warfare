// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
#include "JsonReadWrite.h"
#include "Gravity.h"

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

	for (int i = 0; i < GravityActors.Num(); i++)
	{
		for (int j = 0; j < GravityActors.Num(); j++)
		{
			if (i == j)
			{
				continue;
			}

			FVector GravityForce = UGravity::CalculateGravityForce(GravityActors[i], GravityActors[j], G);

			GravityActors[i]->AddForce(GravityForce);
		}
	}

	for (ACPP_GravityActor* GravityActor : GravityActors)
	{
		UGravity::SemiImplicitEulerIntegrator(GravityActor, DeltaTime);
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
	G = SimulationConfig.GravitationalConstant * SimulationConfig.Time * SimulationConfig.Time;
	for (ACPP_GravityActor* GravityActor : GravityActors)
	{
		if (GravityActor->ActorHasTag("Earth"))
		{
			GravityActor->SetMass(SimulationConfig.Earth.Mass);
			GravityActor->SetSize(SimulationConfig.Earth.Size);
			GravityActor->SetLocation(SimulationConfig.Earth.Location);
			GravityActor->SetInitialVelocity(SimulationConfig.Earth.InitialVelocity * SimulationConfig.Time);
			continue;
		}

		for (FGravityBodyStruct& SatelliteConfig : SimulationConfig.Satellites)
		{
			// TODO: check if it is the right satellite
			GravityActor->SetMass(SatelliteConfig.Mass);
			GravityActor->SetSize(SatelliteConfig.Size);
			GravityActor->SetLocation(SatelliteConfig.Location);
			GravityActor->SetInitialVelocity(SatelliteConfig.InitialVelocity * SimulationConfig.Time);
		}
	}
}
