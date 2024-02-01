// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Universe.generated.h"


// Forward Declarations
class ACPP_Planet;


USTRUCT(BlueprintType)
struct FOrbitalState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;
};

/** Group of orbital elements necessary to uniquely describe an orbit */
USTRUCT(BlueprintType)
struct FOrbitalElements
{
	GENERATED_BODY()

	/** Eccentricity */
	UPROPERTY(BlueprintReadWrite)
	float Eccentricity;

	/** Semi-major axis */
	UPROPERTY(BlueprintReadWrite)
	float SemiMajorAxis;

	/** Inclination in degrees */
	UPROPERTY(BlueprintReadWrite)
	float Inclination;

	/** Longitude of ascending node in degrees */
	UPROPERTY(BlueprintReadWrite)
	float LongitudeOfAscendingNode;

	/** Argument of periapsis in degrees */
	UPROPERTY(BlueprintReadWrite)
	float ArgumentOfPeriapsis;

	/** Mean anomaly at epoch in degrees */
	UPROPERTY(BlueprintReadWrite)
	float MeanAnomaly;

	/** Time at which the body starts orbiting in ISO 8601 */
	UPROPERTY(BlueprintReadWrite)
	FString Epoch;
};

USTRUCT(BlueprintType)
struct FGeographicCoordinates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Altitude;
};


/**
 * 
 */
UCLASS()
class SPACEWARFARE_API UUniverse : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
	static FOrbitalState ConvertOrbitalElementsToOrbitalState(FOrbitalElements OrbitalElements, double GM);

    UFUNCTION(BlueprintCallable)
	static double GetEarthRotationAngle(double JulianDay);

	UFUNCTION(BlueprintCallable)
	static FGeographicCoordinates ConvertECILocationToGeographicCoordinates(ACPP_Planet* Planet, FVector Location);

    UFUNCTION(BlueprintCallable)
    static FVector ConvertGeographicCoordinatesToECILocation(ACPP_Planet* Planet, const FGeographicCoordinates& GeographicCoordinates);
};
