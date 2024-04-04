// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CPP_SatelliteCommands.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SPACEWARFARE_API UCPP_SatelliteCommand : public UObject
{
	GENERATED_BODY()

public:
    FDateTime ExecutionTime;

    virtual void Execute(class ACPP_Satellite* Satellite) PURE_VIRTUAL(UCPP_SatelliteCommand::Execute,);
    virtual FSatelliteCommandData SerializeToStruct() PURE_VIRTUAL(UCPP_SatelliteCommand::SerializeToStruct, return FSatelliteCommandData(););
};

UCLASS()
class SPACEWARFARE_API UCPP_TorqueCommand : public UCPP_SatelliteCommand
{
	GENERATED_BODY()

public:
    FVector Torque;

    virtual void Execute(class ACPP_Satellite* Satellite) override;
    virtual FSatelliteCommandData SerializeToStruct() override;
};

UCLASS()
class SPACEWARFARE_API UCPP_ThrustCommand : public UCPP_SatelliteCommand
{
	GENERATED_BODY()

public:
    FVector Force;
    float Duration;

    virtual void Execute(class ACPP_Satellite* Satellite) override;
    virtual FSatelliteCommandData SerializeToStruct() override;
};
