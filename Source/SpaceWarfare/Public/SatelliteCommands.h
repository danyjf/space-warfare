#pragma once

#include "CoreMinimal.h"
#include "SatelliteCommands.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommand
{
    GENERATED_BODY();
};

USTRUCT(BlueprintType)
struct FTorqueCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FVector Torque;
};

USTRUCT(BlueprintType)
struct FThrustForDurationCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    // Time at which to execute this command
    //UPROPERTY()
    //FDateTime ExecutionTime;

    // Apply the thrust in any direction, world coordinates,
    // the magnitude is the strength
    UPROPERTY(BlueprintReadWrite)
    FVector Force;
    
    // How long to apply the thrust for;
    // 0 means apply until told to deactivate
    UPROPERTY(BlueprintReadWrite)
    float Duration;
};
