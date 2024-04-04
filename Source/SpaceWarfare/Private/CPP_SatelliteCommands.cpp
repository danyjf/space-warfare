// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteCommands.h"
#include "CPP_Satellite.h"
#include "CPP_Thruster.h"

#include "Kismet/KismetMathLibrary.h"

// Torque command
void UCPP_TorqueCommand::Execute(ACPP_Satellite* Satellite)
{
    FVector LocalTorque = UKismetMathLibrary::TransformDirection(Satellite->GetActorTransform(), Torque);

    Satellite->StaticMeshComponent->AddTorqueInDegrees(LocalTorque, NAME_None, true);
}

FSatelliteCommandData UCPP_TorqueCommand::SerializeToStruct()
{
    FTorqueCommandData TorqueCommandData;
    TorqueCommandData.ExecutionTime = ExecutionTime.ToIso8601();
    TorqueCommandData.Torque = Torque;
    return TorqueCommandData;
}

void UCPP_TorqueCommand::DeserializeFromStruct(const FTorqueCommandData& CommandData)
{
	FDateTime::ParseIso8601(*CommandData.ExecutionTime, ExecutionTime);
    Torque = CommandData.Torque;
}

// Thrust command
void UCPP_ThrustCommand::Execute(ACPP_Satellite* Satellite)
{
    UCPP_Thruster* Thruster = Cast<UCPP_Thruster>(Satellite->FindComponentByClass(UCPP_Thruster::StaticClass()));
    Thruster->SetThrusterDirectionInECICoordinates(Force);
    Thruster->ActivateThruster(Force.Size(), Duration);
}

FSatelliteCommandData UCPP_ThrustCommand::SerializeToStruct()
{
    FThrustCommandData ThrustCommandData;
    ThrustCommandData.ExecutionTime = ExecutionTime.ToIso8601();
    ThrustCommandData.Force = Force;
    ThrustCommandData.Duration = Duration;
    return ThrustCommandData;
}

void UCPP_ThrustCommand::DeserializeFromStruct(const FThrustCommandData& CommandData)
{
	FDateTime::ParseIso8601(*CommandData.ExecutionTime, ExecutionTime);
    Force = CommandData.Force;
    Duration = CommandData.Duration;
}
