// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteLauncherSpawner.h"
#include "CPP_SatelliteLauncher.h"
#include "CPP_Planet.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_BotGameMode.h"
#include "CPP_CasualGameMode.h"
#include "CPP_GroundStation.h"
#include "CPP_GroundStationRepresentation.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_SatelliteLauncherSpawner::ACPP_SatelliteLauncherSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    bIsChoosingLocation = false;
    bCanSpawnSatelliteLauncher = true;
}

// Called when the game starts or when spawned
void ACPP_SatelliteLauncherSpawner::BeginPlay()
{
	Super::BeginPlay();
    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

// Called every frame
void ACPP_SatelliteLauncherSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_SatelliteLauncherSpawner::SpawnSatelliteLauncherRepresentation(const FVector& Location)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(GetOwner());

    bIsChoosingLocation = true;

    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_GroundStationRepresentation* GroundStation = GetWorld()->SpawnActorDeferred<ACPP_GroundStationRepresentation>(GroundStationRepresentationBlueprint, SpawnLocation, PlayerController);
    GroundStation->Planet = Planet;
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    GroundStation->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);

    GroundStation->SetConeColor(FLinearColor::Blue);

    GroundStationRepresentation = GroundStation;

    OnUpdateSatelliteLauncherRepresentation.Broadcast();
}

void ACPP_SatelliteLauncherSpawner::UpdateSatelliteLauncherLocation(const FVector& Location)
{
    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStationRepresentation->SetGeographicCoordinates(GeographicCoordinates);

    OnUpdateSatelliteLauncherRepresentation.Broadcast();
}

void ACPP_SatelliteLauncherSpawner::ServerSpawnSatelliteLauncher_Implementation(const FVector& Location)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(GetOwner());

    if (PlayerController->PlayerStatus != EPlayerStatus::PLACING_SATELLITE_LAUNCHER)
    {
        return;
    }
    
    // Create satellite launcher
    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_SatelliteLauncher* SatelliteLauncher = GetWorld()->SpawnActorDeferred<ACPP_SatelliteLauncher>(SatelliteLauncherBlueprint, SpawnLocation, PlayerController);
    SatelliteLauncher->Planet = Planet;
    SatelliteLauncher->OwnerPlayerID = OwnerPlayerID;
    SatelliteLauncher->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    SatelliteLauncher->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    SatelliteLauncher->SetGeographicCoordinates(GeographicCoordinates);

    // When not on casual game mode create ground station on same location as satellite launcher
    if (Cast<ACPP_CasualGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        ACPP_GroundStationRepresentation* GroundStation = GetWorld()->SpawnActorDeferred<ACPP_GroundStationRepresentation>(GroundStationRepresentationBlueprint, SpawnLocation, PlayerController);
        GroundStation->Planet = Planet;
        GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
        GroundStation->FinishSpawning(SpawnLocation);

        GroundStation->SetGeographicCoordinates(GeographicCoordinates);
    }
    else
    {
        ACPP_GroundStation* GroundStation = GetWorld()->SpawnActorDeferred<ACPP_GroundStation>(GroundStationBlueprint, SpawnLocation, PlayerController);
        GroundStation->Planet = Planet;
        GroundStation->OwnerPlayerID = OwnerPlayerID;
        GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
        GroundStation->FinishSpawning(SpawnLocation);

        GroundStation->SetGeographicCoordinates(GeographicCoordinates);
    }

    if (Cast<ACPP_BotGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        PlayerController->PlayerStatus = EPlayerStatus::PLACING_GROUND_STATIONS;
    }
}
