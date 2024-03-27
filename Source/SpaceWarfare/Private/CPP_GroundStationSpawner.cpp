// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationSpawner.h"
#include "CPP_GroundStation.h"
#include "CPP_GroundStationRepresentation.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_Planet.h"
#include "Universe.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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
    if (!HasAuthority())
    {
        CameraOrbitController = Cast<ACPP_CameraOrbitController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    }
    else
    {
        CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());
    }

    if (CameraOrbitController->IsLocalPlayerController())
    {
        CameraOrbitController->OnAllPlayersFinishedPlacingGroundStations.AddDynamic(this, &ACPP_GroundStationSpawner::DestroyGroundStationRepresentation);
    }
}

// Called every frame
void ACPP_GroundStationSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStationSpawner::SpawnGroundStationAtRepresentationLocation()
{
    if (!GroundStationRepresentation)
    {
        return;
    }

    ServerSpawnGroundStation(GroundStationRepresentation->GetActorLocation());

    GroundStationRepresentation->Destroy();
    GroundStationRepresentation = nullptr;
    bIsChoosingLocation = false;
}

void ACPP_GroundStationSpawner::ServerSpawnGroundStation_Implementation(FVector Location)
{
    ACPP_CameraOrbitController* PlayerController = Cast<ACPP_CameraOrbitController>(GetOwner());

    if (PlayerController->PlayerStatus != EPlayerStatus::PLACING_GROUND_STATIONS)
    {
        return;
    }
    
    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_GroundStation* GroundStation = GetWorld()->SpawnActorDeferred<ACPP_GroundStation>(GroundStationBlueprint, SpawnLocation, PlayerController);
    GroundStation->Planet = Planet;
    GroundStation->OwnerPlayerID = OwnerPlayerID;
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    GroundStation->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);

    GroundStation->UpdateCost();

    if (PlayerController->Currency < GroundStation->Cost)
    {
        UKismetSystemLibrary::PrintString(GetWorld(), "Not enough money to purchase ground station!!!");
        GroundStation->Destroy();
        return;
    }

    PlayerController->SpendCurrency(GroundStation->Cost);
}

void ACPP_GroundStationSpawner::SpawnGroundStationRepresentation(FVector Location)
{
    bIsChoosingLocation = true;

    FTransform SpawnLocation(FVector(0.0f, 0.0f, 0.0f));
    ACPP_GroundStationRepresentation* GroundStation = GetWorld()->SpawnActorDeferred<ACPP_GroundStationRepresentation>(GroundStationRepresentationBlueprint, SpawnLocation);
    GroundStation->Planet = Planet;
    GroundStation->AttachToActor(Planet, FAttachmentTransformRules::KeepWorldTransform);
    GroundStation->FinishSpawning(SpawnLocation);

    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStation->SetGeographicCoordinates(GeographicCoordinates);

    GroundStation->SetConeColor(FLinearColor::Blue);

    GroundStationRepresentation = GroundStation;
    GroundStationRepresentation->UpdateCost();

    OnUpdateGroundStationRepresentation.Broadcast();
}

void ACPP_GroundStationSpawner::UpdateGroundStationRepresentationLocation(FVector Location)
{
    FGeographicCoordinates GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(Planet, Location);
    GeographicCoordinates.Altitude = 0.0f;
    GroundStationRepresentation->SetGeographicCoordinates(GeographicCoordinates);
    GroundStationRepresentation->UpdateCost();

    OnUpdateGroundStationRepresentation.Broadcast();
}

void ACPP_GroundStationSpawner::DestroyGroundStationRepresentation()
{
    if (GroundStationRepresentation)
    {
        GroundStationRepresentation->Destroy();
        GroundStationRepresentation = nullptr;
    }
}
