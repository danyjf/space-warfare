// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"


void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates()
{
	return GeographicCoordinates;
}
