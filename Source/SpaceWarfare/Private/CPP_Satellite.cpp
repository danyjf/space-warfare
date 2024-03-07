// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Satellite.h"
#include "CPP_SimulationGameMode.h"
#include "CPP_GravityComponent.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GravityManager.h"

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
}

// Called when the game starts or when spawned
void ACPP_Satellite::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
	    SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACPP_Satellite::OnComponentHit);
    }
}

void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (HasAuthority())
    {
	    GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());
    }
}

void ACPP_Satellite::Destroyed()
{
    Super::Destroyed();

    if (!HasAuthority() || !SimulationGameMode)
    {
        return;
    }

    SimulationGameMode->GravityManager->GravityComponents.Remove(GravityComponent);

    // TODO: Change later, this is to remove the satellite on all players when it is destroyed
    TArray<AActor*> GroundStationManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass(), GroundStationManagers);
    for (AActor* Actor : GroundStationManagers)
    {
        ACPP_GroundStationManager* GroundStationManager = Cast<ACPP_GroundStationManager>(Actor);
        GroundStationManager->ClientSatelliteDestroyed(Name);
    }
}

void ACPP_Satellite::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (ACPP_Satellite* HitSatellite = Cast<ACPP_Satellite>(OtherActor))
    {
        HitSatellite->Destroy();
    }
    this->Destroy();
}

const FGeographicCoordinates& ACPP_Satellite::GetGeographicCoordinates() const
{
	return GeographicCoordinates;
}

const FSatelliteStatus& ACPP_Satellite::GetSatelliteStatus()
{
    SatelliteStatus.Position = GetActorLocation();
    SatelliteStatus.Rotation = GetActorRotation();
    SatelliteStatus.Velocity = GetVelocity();

	return SatelliteStatus;
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
