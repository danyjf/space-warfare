// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationSpawner.h"
#include "CPP_GroundStation.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_Planet.h"
#include "Universe.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_GroundStationSpawner::ACPP_GroundStationSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    bIsChoosingLocation = false;
}

// Called when the game starts or when spawned
void ACPP_GroundStationSpawner::BeginPlay()
{
	Super::BeginPlay();

    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

// Called every frame
void ACPP_GroundStationSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStationSpawner::SpawnGroundStationAtVisualizationLocation()
{
    if (!GroundStationVisualization)
    {
        return;
    }

    GroundStationVisualization->Destroy();
    bIsChoosingLocation = false;

    ServerSpawnGroundStation(GroundStationVisualization->GetActorLocation());
}

void ACPP_GroundStationSpawner::ServerSpawnGroundStation_Implementation(FVector Location)
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());

    if (CameraOrbitController->PlayerStatus != EPlayerStatus::PLACING_GROUND_STATIONS)
    {
        return;
    }

    ACPP_GroundStation* GroundStation = Cast<ACPP_GroundStation>(GetWorld()->SpawnActor(GroundStationBlueprint));
    GroundStation->Planet = Planet;
    GroundStation->OwnerPlayerID = OwnerPlayerID;
    GroundStation->SetOwner(CameraOrbitController);
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);
}

void ACPP_GroundStationSpawner::SpawnGroundStationVisualization(FVector Location)
{
    bIsChoosingLocation = true;
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());

    ACPP_GroundStation* GroundStation = Cast<ACPP_GroundStation>(GetWorld()->SpawnActor(GroundStationBlueprint));
    GroundStation->SetReplicates(false);
    GroundStation->Planet = Planet;
    GroundStation->OwnerPlayerID = OwnerPlayerID;
    GroundStation->SetOwner(CameraOrbitController);
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);

    GroundStationVisualization = GroundStation;
}

void ACPP_GroundStationSpawner::UpdateGroundStationVisualizationLocation(FVector Location)
{
    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStationVisualization->SetGeographicCoordinates(GeographicCoordinates);
}
