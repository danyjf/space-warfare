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

void UCPP_SatelliteCommandManager::HandleNewCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand)
{
    if (!GroundStationManager->OverpassingSatellites.Contains(SatelliteID))
    {
        StoreSatelliteCommand(SatelliteID, SatelliteCommand);
        return;
    }

    SendCommandToSatellite(SatelliteID, SatelliteCommand);
}

void UCPP_SatelliteCommandManager::SendCommandToSatellite(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand)
{
    GroundStationManager->OverpassingSatellites[SatelliteID]->AddCommand(SatelliteCommand);
}

// This is only called on the server
void UCPP_SatelliteCommandManager::SendPendingCommandsToSatellite(const int SatelliteID)
{
    if (!PendingSatelliteCommands.Contains(SatelliteID))
    {
        return;
    }

    for (UCPP_SatelliteCommand* Command : PendingSatelliteCommands[SatelliteID].CommandList)
    {
        SendCommandToSatellite(SatelliteID, Command);
    }

    PendingSatelliteCommands.Remove(SatelliteID);
    ClientRemovePendingSatelliteCommand();
}

void UCPP_SatelliteCommandManager::PrintPendingSatelliteCommands()
{
    for (auto& Elem : PendingSatelliteCommands)
    {
        for (UCPP_SatelliteCommand* SatelliteCommand : Elem.Value.CommandList)
        {
            UKismetSystemLibrary::PrintString(GetWorld(), SatelliteCommand->ExecutionTime.ToIso8601());
        }
    }
}

void UCPP_SatelliteCommandManager::StoreSatelliteCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand)
{
    if (!PendingSatelliteCommands.Contains(SatelliteID))
    {
        FSatelliteCommandList SatelliteCommandList;
        SatelliteCommandList.CommandList.Add(SatelliteCommand);
        PendingSatelliteCommands.Emplace(SatelliteID, SatelliteCommandList);
        return;
    }
    PendingSatelliteCommands[SatelliteID].CommandList.Add(SatelliteCommand);
}

void UCPP_SatelliteCommandManager::ClientRemovePendingSatelliteCommand_Implementation()
{
    // Remove the command from the command list on the client
}

void UCPP_SatelliteCommandManager::ServerSatelliteTorqueCommand_Implementation(const int SatelliteID, const FTorqueCommandData& TorqueCommandData)
{
    // Check if the satellite id exists
    if (!GroundStationManager->TrackedSatellites.Contains(SatelliteID))
    {
        return;
    }

    // Check if satellite belongs to the player
    if (GroundStationManager->TrackedSatellites[SatelliteID].OwnerID != GroundStationManager->OwnerPlayerID)
    {
        return;
    }

    UCPP_TorqueCommand* TorqueCommand = NewObject<UCPP_TorqueCommand>();
    TorqueCommand->DeserializeFromStruct(TorqueCommandData);
    HandleNewCommand(SatelliteID, TorqueCommand);
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustCommand_Implementation(const int SatelliteID, const FThrustCommandData& ThrustCommandData)
{
    // Check if the satellite id exists
    if (!GroundStationManager->TrackedSatellites.Contains(SatelliteID))
    {
        return;
    }

    // Check if satellite belongs to the player
    if (GroundStationManager->TrackedSatellites[SatelliteID].OwnerID != GroundStationManager->OwnerPlayerID)
    {
        return;
    }

    UCPP_ThrustCommand* ThrustCommand = NewObject<UCPP_ThrustCommand>();
    ThrustCommand->DeserializeFromStruct(ThrustCommandData);
    HandleNewCommand(SatelliteID, ThrustCommand);
}

void UCPP_SatelliteCommandManager::ServerSatelliteThrustDeactivate_Implementation(const int SatelliteID)
{
    if (!GroundStationManager->OverpassingSatellites.Contains(SatelliteID))
    {
        return;
    }

    ACPP_Satellite* Satellite = GroundStationManager->OverpassingSatellites[SatelliteID];
    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));

    Thruster->DeactivateThruster();
}
