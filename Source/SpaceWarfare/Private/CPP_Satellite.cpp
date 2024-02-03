// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"
#include "CPP_SimulationGameMode.h"
#include "CPP_GravityComponent.h"

#include "Kismet/GameplayStatics.h"


ACPP_Satellite::ACPP_Satellite()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACPP_Satellite::BeginPlay()
{
	Super::BeginPlay();

	SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    GravityComponent = FindComponentByClass<UCPP_GravityComponent>();
}

void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());
    SatelliteStatus.Position = GetActorLocation();
    SatelliteStatus.Rotation = GetActorRotation();
    SatelliteStatus.Velocity = GravityComponent->GetVelocity();
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates() const
{
	return GeographicCoordinates;
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
