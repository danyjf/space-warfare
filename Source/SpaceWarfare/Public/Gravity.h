// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Gravity.generated.h"


// Forward Declarations
class ACPP_GravityActor;

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
};
