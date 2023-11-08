// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"


void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Longitude: %f; Latitude: %f; Altitude: %f"), GeographicCoordinates.Longitude, GeographicCoordinates.Latitude, GeographicCoordinates.Altitude);
}
