// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationSpawner.h"
#include "CPP_GroundStation.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_Planet.h"
#include "Universe.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACPP_GroundStationSpawner::ACPP_GroundStationSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_GroundStationSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_GroundStationSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStationSpawner::ServerSpawnGroundStation_Implementation(FVector Location)
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());

    ACPP_GroundStation* GroundStation = Cast<ACPP_GroundStation>(GetWorld()->SpawnActor(GroundStationBlueprint));
    GroundStation->Planet = Planet;
    GroundStation->OwnerPlayerID = OwnerPlayerID;
    GroundStation->SetOwner(CameraOrbitController);
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);

    UKismetSystemLibrary::PrintString(GetWorld(), Location.ToString());
}
