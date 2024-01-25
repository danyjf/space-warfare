// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"
#include "CPP_SimulationGameMode.h"


void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());
}

void ACPP_Satellite::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
}

void ACPP_Satellite::UpdateGravityForce()
{
	//AddForce(UUniverse::CalculateGravityForce(this, SimulationGameMode->Planet));
	//for (ACPP_Satellite* Satellite : SimulationGameMode->Satellites)
	//{
	//	if (Satellite != this)
	//	{
	//		FVector GravityForce = UUniverse::CalculateGravityForce(this, Satellite, SimulationGameMode->GravitationalConstant);
	//		AddForce(GravityForce);
	//	}
	//}
}

void ACPP_Satellite::PrintGeographicCoordinates()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Current Epoch: %s; Longitude: %f; Latitude: %f; Altitude: %f"),
		*SimulationGameMode->CurrentEpoch.ToString(TEXT("%Y-%m-%d %H:%M:%S+0000")),
		GeographicCoordinates.Longitude,
		GeographicCoordinates.Latitude,
		GeographicCoordinates.Altitude
	);
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates() const
{
	return GeographicCoordinates;
}
