// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_SimulationGameMode.generated.h"


// Forward Declarations
class ACPP_GravityManager;


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
	double GM;

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
class SPACEWARFARE_API ACPP_SimulationGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACPP_GravityManager* GravityManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SatelliteBlueprintClass;	// Satellite BP used to spawn satellites on initialization from config

    UPROPERTY(BlueprintReadOnly)
	FSimulationConfigStruct SimulationConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeScale;

    UPROPERTY(BlueprintReadOnly)
	FDateTime CurrentEpoch;

	ACPP_SimulationGameMode();

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	UFUNCTION(BlueprintCallable)
	void InitializeSimulationVariables();

private:
	FDateTime InitialEpoch;
	float ElapsedTime;
};
