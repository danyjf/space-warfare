// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_Satellite.h"
#include "CPP_Planet.h"
#include "CPP_Asteroid.h"
#include "CPP_OrbitSpline.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_GravityComponent.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GravityManager.h"
#include "CPP_SatelliteCommands.h"
#include "CPP_SatelliteCommandManager.h"
#include "CPP_GameState.h"
#include "Universe.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


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
        SatelliteCommandManager = MultiplayerGameMode->GetGroundStationManagers()[OwnerPlayerID]->SatelliteCommandManager;
        SatelliteID = MultiplayerGameMode->NewSatelliteID();
        //MultiplayerGameMode->AllSatellites.Emplace(SatelliteID, this);
        StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACPP_Satellite::OnComponentHit);
    }
    
    GetWorld()->GetGameState<ACPP_GameState>()->AllSatellites.Emplace(SatelliteID, this);

    // Create the orbit spline of the satellite
    OrbitSpline = Cast<ACPP_OrbitSpline>(GetWorld()->SpawnActor(OrbitSplineBlueprint));

    OrbitalState = FOrbitalState(GetActorLocation(), GetVelocity());
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, OrbitingPlanet->GravityComponent->GetGravitationalParameter());

    if (IsOwnedBy(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        OrbitSpline->SetColor(FLinearColor::Green);
    }
    else
    {
        OrbitSpline->SetColor(FLinearColor::Red);
    }
}

void ACPP_Satellite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (HasAuthority())
    {
	    GeographicCoordinates = UUniverse::ConvertECILocationToGeographicCoordinates(OrbitingPlanet, GetActorLocation());
        OrbitalState.Location = GetActorLocation();
        OrbitalState.Velocity = GetVelocity();

        // Check if it's time to execute the command, the first on the list is always next
        if (!Commands.IsEmpty() && MultiplayerGameMode->GetGameState<ACPP_GameState>()->CurrentEpoch >= Commands[0]->ExecutionTime)
        {
            Commands[0]->Execute(this);
            Commands.RemoveAt(0);
            SatelliteCommandManager->ClientSatelliteExecutedCommand(SatelliteID);
        }
    }

    // Update the satellite orbit
    FOrbitalElements OrbitalElements = UUniverse::ConvertOrbitalStateToOrbitalElements(OrbitalState, OrbitingPlanet->GravityComponent->GetGravitationalParameter());
    OrbitSpline->UpdateOrbit(OrbitalElements, OrbitingPlanet);
}

void ACPP_Satellite::Destroyed()
{
    Super::Destroyed();

    // Do this check because the Destroyed event gets called by the editor when compiling
    if (GetWorld()->GetGameState<ACPP_GameState>())
    {
        GetWorld()->GetGameState<ACPP_GameState>()->AllSatellites.Remove(SatelliteID);

        OrbitSpline->Destroy();
    }

    if (!HasAuthority() || !MultiplayerGameMode)
    {
        return;
    }

    // TODO: Change later, this is to remove the satellite on all players when it is destroyed
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->ClientSatelliteDestroyed(GetSatelliteID());
        GroundStationManager->TrackedSatellites.Remove(GetSatelliteID());
        GroundStationManager->OverpassingSatellites.Remove(GetSatelliteID());
    }
}

void ACPP_Satellite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_Satellite, OrbitingPlanet, COND_InitialOnly);
    DOREPLIFETIME(ACPP_Satellite, OrbitalState);
    DOREPLIFETIME(ACPP_Satellite, SatelliteID);
}

void ACPP_Satellite::OnRep_SatelliteID()
{
    if (!GetWorld()->GetGameState<ACPP_GameState>()->AllSatellites.Contains(SatelliteID))
    {
        GetWorld()->GetGameState<ACPP_GameState>()->AllSatellites.Emplace(SatelliteID, this);
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
    SatelliteInfo.Epoch = MultiplayerGameMode->GetGameState<ACPP_GameState>()->CurrentEpoch;

	return SatelliteInfo;
}

void ACPP_Satellite::AddCommand(UCPP_SatelliteCommand* Command)
{
    Commands.Add(Command);

    Algo::Sort(Commands, [](UCPP_SatelliteCommand* CommandA, UCPP_SatelliteCommand* CommandB) {
        return CommandA->ExecutionTime < CommandB->ExecutionTime;
    });
}

void ACPP_Satellite::PrintGeographicCoordinates()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Current Epoch: %s; Longitude: %f; Latitude: %f; Altitude: %f"),
		*MultiplayerGameMode->GetGameState<ACPP_GameState>()->CurrentEpoch.ToString(TEXT("%Y-%m-%d %H:%M:%S+0000")),
		GeographicCoordinates.Longitude,
		GeographicCoordinates.Latitude,
		GeographicCoordinates.Altitude
	);
}

void ACPP_Satellite::PrintCommands()
{
    for (UCPP_SatelliteCommand* Command : Commands)
    {
        if (Cast<UCPP_ThrustCommand>(Command))
        {
            UKismetSystemLibrary::PrintString(GetWorld(), "Thrust Command -> " + Command->ExecutionTime.ToIso8601());
        }
    }
}
