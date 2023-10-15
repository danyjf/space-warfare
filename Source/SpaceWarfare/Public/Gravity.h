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

USTRUCT(BlueprintType)
struct FOrbitalElements
{
	GENERATED_BODY()

	UPROPERTY()
	float e;	// Eccentricity	

	UPROPERTY()
	float a;	// Semi-major axis

	UPROPERTY()
	float i;	// Inclination

	UPROPERTY()
	float O;	// Longitude of ascending node

	UPROPERTY()
	float w;	// Argument of periapsis

	UPROPERTY()
	float M;	// Mean anomaly at epoch
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
