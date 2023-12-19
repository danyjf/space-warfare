// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Universe.generated.h"


// Forward Declarations
class ACPP_GravityActor;
class ACPP_Planet;
class ACPP_Satellite;


USTRUCT(BlueprintType)
struct FOrbitalState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;
};

/** Group of orbital elements necessary to uniquely describe an orbit */
USTRUCT(BlueprintType)
struct FOrbitalElements
{
	GENERATED_BODY()

	/** Eccentricity */
	UPROPERTY()
	float Eccentricity;

	/** Semi-major axis */
	UPROPERTY()
	float SemiMajorAxis;

	/** Inclination in degrees */
	UPROPERTY()
	float Inclination;

	/** Longitude of ascending node in degrees */
	UPROPERTY()
	float LongitudeOfAscendingNode;

	/** Argument of periapsis in degrees */
	UPROPERTY()
	float ArgumentOfPeriapsis;

	/** Mean anomaly at epoch in degrees */
	UPROPERTY()
	float MeanAnomaly;

	/** Time at which the body starts orbiting in ISO 8601 */
	UPROPERTY()
	FString Epoch;
};

USTRUCT(BlueprintType)
struct FGeographicCoordinates
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Latitude;

	UPROPERTY(BlueprintReadWrite)
	float Longitude;

	UPROPERTY(BlueprintReadWrite)
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
	static FVector CalculateGravityForce(ACPP_GravityActor* ExertedOn, ACPP_GravityActor* ExertedBy, double GravitationalConstant);
	static FVector CalculateGravityForce(ACPP_Satellite* Satellite, ACPP_Planet* Planet);
	static void SemiImplicitEulerIntegrator(ACPP_GravityActor* GravityActor, float DeltaTime);
	static void LeapFrogIntegrator(ACPP_GravityActor* GravityActor, float DeltaTime);
	static FOrbitalState ConvertOrbitalElementsToOrbitalState(FOrbitalElements OrbitalElements, double GM);
	static double GetEarthRotationAngle(double JulianDay);

	UFUNCTION(BlueprintCallable)
	static FGeographicCoordinates ConvertECILocationToGeographicCoordinates(ACPP_Planet* Planet, FVector Location);
};
