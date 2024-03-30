// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"
#include "CPP_Planet.h"
#include "CPP_Asteroid.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_GravityComponent.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GravityManager.h"
#include "Universe.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


ACPP_Satellite::ACPP_Satellite()
{
    PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetLinearDamping(0.0f);
    StaticMeshComponent->SetAngularDamping(0.0f);
    StaticMeshComponent->SetEnableGravity(false);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    RootComponent = StaticMeshComponent;

    GravityComponent = CreateDefaultSubobject<UCPP_GravityComponent>(TEXT("Gravity"));

    SatelliteInfo = FSatelliteInfo();
}

// Called when the game starts or when spawned
void ACPP_Satellite::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
	    MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACPP_Satellite::OnComponentHit);
    }
}

void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (HasAuthority())
    {
	    GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());

        // TODO: Check if it's time to execute the command
        if (MultiplayerGameMode->CurrentEpoch <= Commands[0])
        {
            //Command->Execute();
        }
    }
}

void ACPP_Satellite::Destroyed()
{
    Super::Destroyed();

    if (!HasAuthority() || !MultiplayerGameMode)
    {
        return;
    }

    // TODO: Change later, this is to remove the satellite on all players when it is destroyed
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->ClientSatelliteDestroyed(GetFName());
    }
}

void ACPP_Satellite::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (ACPP_Satellite* HitSatellite = Cast<ACPP_Satellite>(OtherActor))
    {
        HitSatellite->Destroy();
        this->Destroy();
    }
    else if (ACPP_Planet* HitPlanet = Cast<ACPP_Planet>(OtherActor))
    {
        this->Destroy();
    }
    else if (ACPP_Asteroid* HitAsteroid = Cast<ACPP_Asteroid>(OtherActor))
    {
        HitAsteroid->Destroy();
    }
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates() const
{
	return GeographicCoordinates;
}

const FSatelliteInfo& ACPP_Satellite::GetSatelliteInfo()
{
    SatelliteInfo.OwnerID = OwnerPlayerID;
    SatelliteInfo.Label = Label;
    SatelliteInfo.Position = GetActorLocation();
    SatelliteInfo.Rotation = GetActorRotation();
    SatelliteInfo.Velocity = GetVelocity();
    SatelliteInfo.Mass = StaticMeshComponent->GetMass();
    SatelliteInfo.Epoch = MultiplayerGameMode->CurrentEpoch;

	return SatelliteInfo;
}

void ACPP_Satellite::AddCommand(const FSatelliteCommand& Command)
{
    Commands.Add(Command);

    Commands.Sort([](const FSatelliteCommand& CommandA, const FSatelliteCommand& CommandB) {
        return CommandA.ExecutionTime < CommandB.ExecutionTime;
    });
}

void ACPP_Satellite::PrintGeographicCoordinates()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Current Epoch: %s; Longitude: %f; Latitude: %f; Altitude: %f"),
		*MultiplayerGameMode->CurrentEpoch.ToString(TEXT("%Y-%m-%d %H:%M:%S+0000")),
		GeographicCoordinates.Longitude,
		GeographicCoordinates.Latitude,
		GeographicCoordinates.Altitude
	);
}
