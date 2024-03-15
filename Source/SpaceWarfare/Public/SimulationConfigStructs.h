#pragma once

#include "CoreMinimal.h"
#include "SimulationConfigStructs.generated.h"

USTRUCT(BlueprintType)
struct FPlanetStruct
{
	GENERATED_BODY();

	UPROPERTY()
	FString Name;

	UPROPERTY()
	double Mass;			// kilograms

	UPROPERTY()
	float Size;				// kilometers

	UPROPERTY()
	double GM;              // gravitational parameter

	UPROPERTY()
	double RotationSpeed;	// degrees per second

	UPROPERTY()
	FString Epoch;			// epoch for earth rotation in ISO 8601
};

USTRUCT(BlueprintType)
struct FSatelliteStruct
{
	GENERATED_BODY();

	UPROPERTY()
	FString Name;

	UPROPERTY()
	double Mass;	// kilograms

	UPROPERTY()
	float Size;		// kilometers

	UPROPERTY()
	FOrbitalElements OrbitalElements;
};

USTRUCT(BlueprintType)
struct FSimulationConfig
{
	GENERATED_BODY();

	UPROPERTY()
	int TimeScale;	// seconds

	UPROPERTY()
	double GravitationalConstant;

	UPROPERTY()
	FPlanetStruct Planet;
};

USTRUCT(BlueprintType)
struct FSatellitesConfig
{
	GENERATED_BODY();

	UPROPERTY()
	TArray<FSatelliteStruct> Satellites;
};
