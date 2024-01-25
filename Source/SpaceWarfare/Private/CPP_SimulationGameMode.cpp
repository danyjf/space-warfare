// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
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

	//for (int substep = 0; substep < 10; substep++)
	//{
		// Calculate gravity forces between planet and all satellites
		//for (ACPP_Satellite* Satellite : Satellites)
		//{
		//	FVector GravityForce = UUniverse::CalculateGravityForce(Satellite, Planet);

		//	Satellite->AddForce(GravityForce);
		//	Planet->AddForce(-GravityForce);
		//}

		// Calculate gravity forces between all satellites
		//for (int i = 0; i < Satellites.Num(); i++)
		//{
		//	for (int j = i + 1; j < Satellites.Num(); j++)
		//	{
		//		FVector GravityForce = UUniverse::CalculateGravityForce(Satellites[i], Satellites[j], GravitationalConstant);

		//		Satellites[i]->AddForce(GravityForce);
		//		Satellites[j]->AddForce(-GravityForce);
		//	}
		//}

		// Apply the forces with semi implicit euler integrator
		//UUniverse::SemiImplicitEulerIntegrator(Planet, ScaledDeltaTime / 10);
		//UUniverse::LeapFrogIntegrator(Planet, ScaledDeltaTime / 10);
		//for (ACPP_Satellite* Satellite : Satellites)
		//{
			//UUniverse::SemiImplicitEulerIntegrator(Satellite, ScaledDeltaTime / 10);
			//UUniverse::LeapFrogIntegrator(Satellite, ScaledDeltaTime / 10);
		//}
	//}

	// Calculate current time
	ElapsedTime += ScaledDeltaTime;
	FTimespan ElapsedEpoch;
	ElapsedEpoch = ElapsedEpoch.FromSeconds(ElapsedTime);

	CurrentEpoch = InitialEpoch;
	CurrentEpoch += ElapsedEpoch;
}

void ACPP_SimulationGameMode::PrintSimulationData()
{
	//const FGeographicCoordinates& GeographicCoordinates = Satellites[0]->GetGeographicCoordinates();
	//UE_LOG(
	//	LogTemp,
	//	Warning,
	//	TEXT("Current Epoch: %s; Longitude: %f; Latitude: %f; Altitude: %f; Simulation Earth Rotation: %f; Correct Earth Rotation: %f"),
	//	*CurrentEpoch.ToString(TEXT("%Y-%m-%d %H:%M:%S+0000")),
	//	GeographicCoordinates.Longitude,
	//	GeographicCoordinates.Latitude,
	//	GeographicCoordinates.Altitude,
	//	Planet->GetActorRotation().Yaw,
	//	FRotator::NormalizeAxis(UUniverse::GetEarthRotationAngle(CurrentEpoch.GetJulianDay()))
	//);
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
    // Setup variables on gravity actors
	//GravitationalConstant = SimulationConfig.GravitationalConstant;
	//TimeScale = SimulationConfig.TimeScale;

	//FDateTime::ParseIso8601(*SimulationConfig.Planet.Epoch, InitialEpoch);

	//Planet->Initialize(
	//	SimulationConfig.Planet.Name, 
	//	SimulationConfig.Planet.Mass, 
	//	SimulationConfig.Planet.Size, 
	//	SimulationConfig.Planet.GM,
	//	SimulationConfig.Planet.RotationSpeed * TimeScale,
	//	InitialEpoch
	//);

	//for (FSatelliteStruct& SatelliteConfig : SimulationConfig.Satellites)
	//{
	//	bool bSatelliteExists = false;
	//	for (ACPP_Satellite* Satellite : Satellites)
	//	{
	//		if (Satellite->Name != SatelliteConfig.Name)
	//		{
	//			continue;
	//		}

	//		bSatelliteExists = true;
	//		FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
	//		Satellite->Initialize(
	//			SatelliteConfig.Name, 
	//			SatelliteConfig.Mass, 
	//			SatelliteConfig.Size, 
	//			OrbitalState.Location, 
	//			OrbitalState.Velocity
	//		);
	//	}

	//	if (!bSatelliteExists)
	//	{
	//		ACPP_Satellite* NewSatellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
	//		NewSatellite->OrbitingPlanet = Planet;

	//		FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(SatelliteConfig.OrbitalElements, SimulationConfig.Planet.GM);
	//		NewSatellite->Initialize(
	//			SatelliteConfig.Name, 
	//			SatelliteConfig.Mass, 
	//			SatelliteConfig.Size, 
	//			OrbitalState.Location, 
	//			OrbitalState.Velocity
	//		);

	//		Satellites.Add(NewSatellite);
	//	}
	//}

    // Setup variables on gravity manager and components
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
        //UE_LOG(LogTemp, Warning, TEXT("%d"), GravityManager->GravityComponents.Num())
    }
}
