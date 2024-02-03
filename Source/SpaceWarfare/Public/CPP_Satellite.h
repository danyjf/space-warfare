// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "CPP_Satellite.generated.h"


// Forward declarations
class ACPP_SimulationGameMode;
class UCPP_GravityComponent;


/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_Satellite : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACPP_Planet* OrbitingPlanet;

    UFUNCTION(BlueprintCallable)
	const FGeographicCoordinates& GetGeographicCoordinates() const;

    UFUNCTION(BlueprintCallable)
    void PrintGeographicCoordinates();

    UFUNCTION(BlueprintCallable)
    const FSatelliteStatus& GetSatelliteStatus() const { return SatelliteStatus; }

    ACPP_Satellite();    
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FGeographicCoordinates GeographicCoordinates;
    FSatelliteStatus SatelliteStatus;
    ACPP_SimulationGameMode* SimulationGameMode;
    UCPP_GravityComponent* GravityComponent;
};
