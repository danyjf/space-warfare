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

    UFUNCTION(BlueprintCallable)
	const FGeographicCoordinates& GetGeographicCoordinates() const;

    UFUNCTION(BlueprintCallable)
    void PrintGeographicCoordinates();

    UFUNCTION(BlueprintCallable)
	virtual void UpdateGravityForce() override;

	virtual void Tick(float DeltaTime) override;
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

private:
	FGeographicCoordinates GeographicCoordinates;
};
