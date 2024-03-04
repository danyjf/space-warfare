// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "CPP_Satellite.generated.h"

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
    int PlayerNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACPP_Planet* OrbitingPlanet;

    class UCPP_GravityComponent* GravityComponent;

    UFUNCTION(BlueprintCallable)
	const FGeographicCoordinates& GetGeographicCoordinates() const;

    UFUNCTION(BlueprintCallable)
    const FSatelliteStatus& GetSatelliteStatus();

    UFUNCTION(BlueprintCallable)
    void PrintGeographicCoordinates();

    ACPP_Satellite();    

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FGeographicCoordinates GeographicCoordinates;
    FSatelliteStatus SatelliteStatus;
    class ACPP_SimulationGameMode* SimulationGameMode;
};
