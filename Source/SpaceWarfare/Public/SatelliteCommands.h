#pragma once

#include "CoreMinimal.h"
#include "SatelliteCommands.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommand
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    FString SatelliteName;
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
