// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteCommandManager.h"
#include "CPP_GroundStationManager.h"
#include "CPP_Thruster.h"
#include "CPP_Satellite.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCPP_SatelliteCommandManager::UCPP_SatelliteCommandManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
    
    GroundStationManager = Cast<ACPP_GroundStationManager>(GetOwner());
}

void UCPP_SatelliteCommandManager::ServerSatelliteTorqueCommand_Implementation(const FTorqueCommand& TorqueCommand)
{
    if (!GroundStationManager->OverpassingSatellites.Contains(TorqueCommand.UniqueID))
    {
        return;
    }

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[TorqueCommand.UniqueID];
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), TorqueCommand.Torque);

    Satellite->StaticMeshComponent->AddTorqueInDegrees(LocalTorque, NAME_None, true);
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustCommand_Implementation(const FThrustCommand& ThrustCommand)
{
    if (!GroundStationManager->OverpassingSatellites.Contains(ThrustCommand.UniqueID))
    {
        return;
    }

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[ThrustCommand.UniqueID];

    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));
    if (ThrustCommand.IsActive)
    {
        Thruster->SetThrusterDirectionInLocalCoordinates(FVector::ForwardVector);
        Thruster->ActivateThruster(100);
    }
    else
    {
        Thruster->DeactivateThruster();
    }
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustForDurationCommand_Implementation(const FName& SatelliteID, const FThrustForDurationCommand& ThrustCommand)
{
    if (!GroundStationManager->OverpassingSatellites.Contains(SatelliteID))
    {
        return;
    }

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[SatelliteID];

    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));
    Thruster->SetThrusterDirectionInECICoordinates(ThrustCommand.Direction);
    Thruster->ActivateThruster(ThrustCommand.Direction.Size(), ThrustCommand.Duration);
}
