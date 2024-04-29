// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_Asteroid.h"
#include "CPP_GravityComponent.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GravityManager.h"
#include "CPP_OrbitSpline.h"
#include "CPP_Planet.h"
#include "CPP_AsteroidSpawner.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACPP_Asteroid::ACPP_Asteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetLinearDamping(0.0f);
    StaticMeshComponent->SetAngularDamping(0.0f);
    StaticMeshComponent->SetEnableGravity(false);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    RootComponent = StaticMeshComponent;

    GravityComponent = CreateDefaultSubobject<UCPP_GravityComponent>(TEXT("Gravity"));
}

// Called when the game starts or when spawned
void ACPP_Asteroid::BeginPlay()
{
	Super::BeginPlay();

    // TODO: I got this to work in a very weird way, it's calling the new asteroid function on 
    // the ground station manager but only the one belonging to the server.
    // Then when it is created on the client it calls the same function but on the client, I
    // guess it is not an RPC at all and it should be done in a different way.
    if (HasAuthority())
    {
	    MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        MultiplayerGameMode->GetGroundStationManagers()[0]->ClientNewAsteroidTracked(GetFName(), this);
    }
    else
    {
        ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass()));
        GroundStationManager->ClientNewAsteroidTracked(GetFName(), this);
    }

    // Create the orbit spline of the satellite
    OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));
    OrbitSpline->SetColor(FLinearColor::Yellow);
}

// Called every frame
void ACPP_Asteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        OrbitalState.Location = GetActorLocation();
        OrbitalState.Velocity = GetVelocity();
    }

    // Update the satellite orbit
    if (OrbitSpline->bIsVisualizationEnabled)
    {
        FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, OrbitingPlanet->GravityComponent->GetGravitationalParameter());
        OrbitSpline->UpdateOrbit(OrbitalElements, OrbitingPlanet);
    }
}

void ACPP_Asteroid::Destroyed()
{
    Super::Destroyed();

    if (!HasAuthority() || !MultiplayerGameMode)
    {
        return;
    }

    ACPP_AsteroidSpawner* AsteroidSpawner = Cast<ACPP_AsteroidSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_AsteroidSpawner::StaticClass()));
    AsteroidSpawner->AsteroidCount--;

    // TODO: Change later, this is to remove the satellite on all players when it is destroyed
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->ClientAsteroidDestroyed(GetFName());
    }
}

void ACPP_Asteroid::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_Asteroid, OrbitingPlanet, COND_InitialOnly);
    DOREPLIFETIME(ACPP_Asteroid, OrbitalState);
}
