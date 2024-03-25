// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_AsteroidSpawner.h"
#include "CPP_Planet.h"
#include "CPP_GravityComponent.h"
#include "CPP_Asteroid.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_GravityManager.h"
#include "CPP_GroundStationManager.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_AsteroidSpawner::ACPP_AsteroidSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_AsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

	MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    SpawnAtPlanet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

// Called every frame
void ACPP_AsteroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_AsteroidSpawner::SpawnAsteroidAtRandomOrbit()
{
    if (MultiplayerGameMode->bWaitingForPlayers)
    {
        return;
    }

    FOrbitalElements OrbitalElements;

    OrbitalElements.Eccentricity = UKismetMathLibrary::RandomFloatInRange(0.0f, 0.03f);
    OrbitalElements.SemiMajorAxis = UKismetMathLibrary::RandomFloatInRange(6800.0f, 9000.0f);
    OrbitalElements.Inclination = UKismetMathLibrary::RandomFloatInRange(0.0f, 180.0f);
    OrbitalElements.LongitudeOfAscendingNode = UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f);
    OrbitalElements.ArgumentOfPeriapsis = UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f);
    OrbitalElements.MeanAnomaly = UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f);
    
    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(OrbitalElements, SpawnAtPlanet->GravityComponent->GetGravitationalParameter());

    double AsteroidMass = 10000.0;
    ACPP_Asteroid* Asteroid = Cast<ACPP_Asteroid>(GetWorld()->SpawnActor(AsteroidBlueprintClass));
    Asteroid->SetActorLocation(OrbitalState.Location);
    Asteroid->GravityComponent->SetVelocity(OrbitalState.Velocity);
    Asteroid->GravityComponent->SetMass(10000.0);
    Asteroid->GravityComponent->SetGravitationalParameter(MultiplayerGameMode->GravityManager->GravitationalConstant * AsteroidMass);

    // TODO: Change later, this is just to show the satellite on all players when it is launched
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->ClientNewAsteroidTracked(Asteroid->GetFName(), Asteroid->GetActorLocation(), Asteroid->GetVelocity());
    }
}
