// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CPP_SatelliteCommands.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SPACEWARFARE_API UCPP_SatelliteCommand : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    FDateTime ExecutionTime;

    virtual void Execute(class ACPP_Satellite* Satellite) PURE_VIRTUAL(UCPP_SatelliteCommand::Execute,);
};

UCLASS()
class SPACEWARFARE_API UCPP_TorqueCommand : public UCPP_SatelliteCommand
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    FVector Torque;

    virtual void Execute(class ACPP_Satellite* Satellite) override;
    virtual void DeserializeFromStruct(const FTorqueCommandData& CommandData);
};

UCLASS()
class SPACEWARFARE_API UCPP_ThrustCommand : public UCPP_SatelliteCommand
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    FVector Force;

    UPROPERTY(BlueprintReadOnly)
    float Duration;

    virtual void Execute(class ACPP_Satellite* Satellite) override;
    virtual void DeserializeFromStruct(const FThrustCommandData& CommandData);
};
