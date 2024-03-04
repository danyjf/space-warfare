// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Universe.generated.h"

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

USTRUCT(BlueprintType)
struct FSatelliteStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Position;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;
};

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
struct FSimulationConfigStruct
{
	GENERATED_BODY();

	UPROPERTY()
	int TimeScale;	// seconds

	UPROPERTY()
	double GravitationalConstant;

	UPROPERTY()
	FPlanetStruct Planet;

	UPROPERTY()
	TArray<FSatelliteStruct> Satellites;
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
	static FOrbitalState ConvertOrbitalElementsToOrbitalState(const FOrbitalElements& OrbitalElements, double GM);

    UFUNCTION(BlueprintCallable)
	static FOrbitalElements ConvertOrbitalStateToOrbitalElements(const FOrbitalState& OrbitalState, double GM);

	UFUNCTION(BlueprintCallable)
	static FGeographicCoordinates ConvertECILocationToGeographicCoordinates(class ACPP_Planet* Planet, FVector Location);

    UFUNCTION(BlueprintCallable)
    static FVector ConvertGeographicCoordinatesToECILocation(class ACPP_Planet* Planet, const FGeographicCoordinates& GeographicCoordinates);

    UFUNCTION(BlueprintCallable)
	static double GetEarthRotationAngle(double JulianDay);

    UFUNCTION(BlueprintCallable)
    static FVector ToRightHandSystem(const FVector& Vector);

    UFUNCTION(BlueprintCallable)
    static FVector ToLeftHandSystem(const FVector& Vector);

    UFUNCTION(BlueprintCallable)
    static float GetMeanAnomaly(float Eccentricity, float TrueAnomaly);

    UFUNCTION(BlueprintCallable)
    static FString OrbitalElementsToString(const FOrbitalElements& OrbitalElements);

    UFUNCTION(BlueprintCallable)
    static FString OrbitalStateToString(const FOrbitalState& OrbitalState);
};
