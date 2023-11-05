// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "CPP_GravityActor.h"
#include "CPP_Satellite.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_Satellite : public ACPP_GravityActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACPP_Planet* OrbitingPlanet;

	virtual void Tick(float DeltaTime) override;

private:
	FGeographicCoordinates GeographicCoordinates;
};
