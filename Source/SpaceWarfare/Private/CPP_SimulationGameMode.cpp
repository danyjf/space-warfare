// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
#include "JsonReadWrite.h"

#include "JsonObjectConverter.h"	// JsonUtilities module
#include "Kismet/KismetSystemLibrary.h"


ACPP_SimulationGameMode::ACPP_SimulationGameMode()
{
	bool bSuccess;
	FString InfoMessage;
	TSharedPtr<FJsonObject> JsonObject = UJsonReadWrite::ReadJson(FPaths::Combine(FPaths::ProjectContentDir(), "SpaceWarfare/Data/SimulationConfig.json"), bSuccess, InfoMessage);

	if (!FJsonObjectConverter::JsonObjectToUStruct<FSimulationConfigStruct >(JsonObject.ToSharedRef(), &SimulationConfig))
	{
		UKismetSystemLibrary::PrintString(this, "Read struct json failed - Was not able to convert the json object to the desired structure. Is it the right format / struct?");
	}
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
