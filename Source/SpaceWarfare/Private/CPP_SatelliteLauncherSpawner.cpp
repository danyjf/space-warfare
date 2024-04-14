// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteLauncherSpawner.h"
#include "CPP_SatelliteLauncher.h"
#include "CPP_Planet.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_BotGameMode.h"
#include "CPP_CasualGameMode.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_SatelliteLauncherSpawner::ACPP_SatelliteLauncherSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
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
    bIsChoosingLocation = true;

    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_SatelliteLauncher* SatelliteLauncher = GetWorld()->SpawnActorDeferred<ACPP_SatelliteLauncher>(SatelliteLauncherBlueprint, SpawnLocation, UGameplayStatics::GetPlayerController(GetWorld(), 0));
    SatelliteLauncher->Planet = Planet;
    SatelliteLauncher->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    SatelliteLauncher->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    SatelliteLauncher->SetGeographicCoordinates(GeographicCoordinates);

    SatelliteLauncherRepresentation = SatelliteLauncher;

    OnUpdateSatelliteLauncherRepresentation.Broadcast();
}

void ACPP_SatelliteLauncherSpawner::UpdateSatelliteLauncherLocation(const FVector& Location)
{
    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    SatelliteLauncherRepresentation->SetGeographicCoordinates(GeographicCoordinates);

    OnUpdateSatelliteLauncherRepresentation.Broadcast();
}

void ACPP_SatelliteLauncherSpawner::ServerSpawnSatelliteLauncher_Implementation(const FVector& Location)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(GetOwner());

    if (PlayerController->PlayerStatus != EPlayerStatus::PLACING_SATELLITE_LAUNCHER)
    {
        return;
    }
    
    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_SatelliteLauncher* SatelliteLauncher = GetWorld()->SpawnActorDeferred<ACPP_SatelliteLauncher>(SatelliteLauncherBlueprint, SpawnLocation, PlayerController);
    SatelliteLauncher->Planet = Planet;
    SatelliteLauncher->OwnerPlayerID = OwnerPlayerID;
    SatelliteLauncher->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    SatelliteLauncher->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    SatelliteLauncher->SetGeographicCoordinates(GeographicCoordinates);

    if (Cast<ACPP_BotGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        PlayerController->PlayerStatus = EPlayerStatus::PLACING_GROUND_STATIONS;
    }
    else if (Cast<ACPP_CasualGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        PlayerController->PlayerStatus = EPlayerStatus::GROUND_STATION_CONTROL;
    }
}
