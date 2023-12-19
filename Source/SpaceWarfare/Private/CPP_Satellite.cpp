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

	float ScaledDeltaTime = DeltaTime * SimulationGameMode->TimeScale;

	AddForce(UUniverse::CalculateGravityForce(this, SimulationGameMode->Planet));
	for (ACPP_Satellite* Satellite : SimulationGameMode->Satellites)
	{
		if (Satellite != this)
		{
			FVector GravityForce = UUniverse::CalculateGravityForce(this, Satellite, SimulationGameMode->GravitationalConstant);
			AddForce(GravityForce);
		}
	}

	UUniverse::SemiImplicitEulerIntegrator(this, ScaledDeltaTime);
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates()
{
	return GeographicCoordinates;
}
