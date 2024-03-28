#pragma once

#include "CoreMinimal.h"
#include "SatelliteCommands.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FName UniqueID;
};

USTRUCT(BlueprintType)
struct FTorqueCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FVector Torque;
};

USTRUCT(BlueprintType)
struct FThrustCommand : public FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    bool IsActive;
};

// Command received from the API calls
USTRUCT(BlueprintType)
struct FThrustForDurationCommand
{
    GENERATED_BODY();

    // Apply the thrust in any direction
    // and the magnitude is the strength
    UPROPERTY()
    FVector Direction;
    
    // How long to apply the thrust for
    UPROPERTY()
    float Duration;
};
