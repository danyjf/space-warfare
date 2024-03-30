// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteCommandManager.h"
#include "CPP_GroundStationManager.h"
#include "CPP_Thruster.h"
#include "CPP_Satellite.h"
#include "CPP_SatelliteCommands.h"

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

void UCPP_SatelliteCommandManager::SendCommandToSatellite(const FName& SatelliteID, UCPP_SatelliteCommand* SatelliteCommand)
{
    GroundStationManager->OverpassingSatellites[SatelliteID]->AddCommand(SatelliteCommand);
}

void UCPP_SatelliteCommandManager::ServerSatelliteTorqueCommand_Implementation(const FName& SatelliteID, const FTorqueCommandData& TorqueCommandData)
{
    UCPP_TorqueCommand* TorqueCommand = NewObject<UCPP_TorqueCommand>();
    if (!GroundStationManager->OverpassingSatellites.Contains(SatelliteID))
    {
        SatelliteCommands.Emplace(SatelliteID, TorqueCommand);
        return;
    }

    SendCommandToSatellite(SatelliteID, TorqueCommand);

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[SatelliteID];
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), TorqueCommandData.Torque);

    Satellite->StaticMeshComponent->AddTorqueInDegrees(LocalTorque, NAME_None, true);
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustForDurationCommand_Implementation(const FName& SatelliteID, const FThrustCommandData& ThrustCommandData, bool bUseLocalCoordinates)
{
    UCPP_ThrustCommand* ThrustCommand = NewObject<UCPP_ThrustCommand>();
    if (!GroundStationManager->OverpassingSatellites.Contains(SatelliteID))
    {
        SatelliteCommands.Emplace(SatelliteID, ThrustCommand);
        return;
    }

    SendCommandToSatellite(SatelliteID, ThrustCommand);

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[SatelliteID];
    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));
    if (bUseLocalCoordinates)
    {
        Thruster->SetThrusterDirectionInLocalCoordinates(FVector::ForwardVector);
    }
    else
    {
        Thruster->SetThrusterDirectionInECICoordinates(ThrustCommandData.Force);
    }
    Thruster->ActivateThruster(ThrustCommandData.Force.Size(), ThrustCommandData.Duration);
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustDeactivate_Implementation(const FName& SatelliteID)
{
    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[SatelliteID];
    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));

    Thruster->DeactivateThruster();
}
