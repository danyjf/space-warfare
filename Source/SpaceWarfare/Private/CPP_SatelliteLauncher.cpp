// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SatelliteLauncher.h"
#include "CPP_Satellite.h"
#include "CPP_Planet.h"
#include "CPP_GravityComponent.h"
#include "CPP_GravityManager.h"
#include "CPP_SimulationGameMode.h"
#include "CPP_GroundStationManager.h"
#include "CPP_CameraOrbitController.h"


#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_SatelliteLauncher::ACPP_SatelliteLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PlayerNumber = 0;
    LaunchCost = 50;    // Millions
}

void ACPP_SatelliteLauncher::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    }
}

// Called every frame
void ACPP_SatelliteLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_SatelliteLauncher::ServerLaunchSatellite_Implementation(FOrbitalElements OrbitalElements, float Size, float Mass, const FString& Name)
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());
    if (CameraOrbitController->Currency < LaunchCost)
    {
        UKismetSystemLibrary::PrintString(GetWorld(), "Not enough money to launch!!!");
        return;
    }

    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(OrbitalElements, Planet->GravityComponent->GetGravitationalParameter());

    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
    Satellite->SetActorLocation(OrbitalState.Location);
    Satellite->SetActorScale3D(FVector(Size));
    Satellite->OrbitingPlanet = Planet;
    Satellite->Name = Name;
    Satellite->PlayerNumber = PlayerNumber;
    Satellite->SetOwner(CameraOrbitController);

    Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
    Satellite->GravityComponent->SetMass(Mass);
    Satellite->GravityComponent->SetGravitationalParameter(SimulationGameMode->GravityManager->GravitationalConstant * Mass);

    CameraOrbitController->SpendCurrency(LaunchCost);

    // TODO: Change later, this is just to show the satellite on all players when it is launched
    TArray<AActor*> GroundStationManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass(), GroundStationManagers);
    for (AActor* Actor : GroundStationManagers)
    {
        ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(Actor);
        GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
    }
}
