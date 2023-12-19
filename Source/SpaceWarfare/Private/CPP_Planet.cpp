// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_SimulationGameMode.h"
#include "CPP_Satellite.h"


void ACPP_Planet::Initialize(FString aName, double Mass, float Size, double aGM, double aRotationSpeed, FDateTime Epoch)
{
	Super::Initialize(aName, Mass, Size, FVector(0.0f), FVector(0.0f));
	GM = aGM;
	RotationSpeed = FRotator(0, -aRotationSpeed, 0);

	FRotator EpochRotationAngle = FRotator(0, UUniverse::GetEarthRotationAngle(Epoch.GetJulianDay()), 0);
	SetActorRotation(EpochRotationAngle);
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}

void ACPP_Planet::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
}

void ACPP_Planet::UpdateGravityForce()
{
	for (ACPP_Satellite* Satellite : SimulationGameMode->Satellites)
	{
		FVector GravityForce = UUniverse::CalculateGravityForce(Satellite, this);

		AddForce(-GravityForce);
	}
}
