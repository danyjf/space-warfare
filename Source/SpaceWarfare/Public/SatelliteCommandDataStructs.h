#pragma once

#include "CoreMinimal.h"
#include "SatelliteCommandDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommandData
{
    GENERATED_BODY();

    // Time at which to execute this command
    UPROPERTY()
    FDateTime ExecutionTime;
};

USTRUCT(BlueprintType)
struct FTorqueCommandData : public FSatelliteCommandData
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FVector Torque;
};

USTRUCT(BlueprintType)
struct FThrustCommandData : public FSatelliteCommandData
{
    GENERATED_BODY();

    // Apply the thrust in any direction, world coordinates,
    // the magnitude is the strength
    UPROPERTY(BlueprintReadWrite)
    FVector Force;
    
    // How long to apply the thrust for;
    // 0 means apply until told to deactivate
    UPROPERTY(BlueprintReadWrite)
    float Duration;
};
