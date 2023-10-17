// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Gravity.generated.h"


// Forward Declarations
class ACPP_GravityActor;


USTRUCT(BlueprintType)
struct FOrbitalState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;
};

/** Group of six orbital elements necessary to uniquely describe an orbit */
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
};


/**
 * 
 */
UCLASS()
class SPACEWARFARE_API UGravity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FVector CalculateGravityForce(ACPP_GravityActor* ExertedOn, ACPP_GravityActor* ExertedBy, double GravitationalConstant);
	static void SemiImplicitEulerIntegrator(ACPP_GravityActor* GravityActor, float DeltaTime);
	static FOrbitalState ConvertOrbitalElementsToOrbitalState(FOrbitalElements OrbitalElements, double GM);
};
