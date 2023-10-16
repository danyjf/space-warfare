// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Gravity.generated.h"


// Forward Declarations
class ACPP_GravityActor;


struct FOrbitalState
{
	FVector Location;
	FVector Velocity;
};

/** Group of six orbital elements necessary to uniquely describe an orbit */
struct FOrbitalElements
{
	/** Eccentricity */
	float e;

	/** Semi-major axis */
	float a;

	/** Inclination in radians */
	float i;

	/** Longitude of ascending node in radians */
	float O;

	/** Argument of periapsis in radians */
	float w;

	/** Mean anomaly at epoch in radians */
	float M;
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
