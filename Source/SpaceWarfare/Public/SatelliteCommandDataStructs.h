#pragma once

#include "CoreMinimal.h"
#include "SatelliteCommandDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommandData
{
    GENERATED_BODY();

    // Time at which to execute this command
    UPROPERTY(BlueprintReadWrite)
    FString ExecutionTime = "0000-00-00T00:00:00.000Z";  // Scheduled execution time in ISO 8601
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
