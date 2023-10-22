// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gravity.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_SimulationGameMode.generated.h"


// Forward Declarations
class ACPP_GravityActor;
class ACPP_Planet;
class ACPP_Satellite;


USTRUCT(BlueprintType)
struct FPlanetStruct
{
	GENERATED_BODY();

	UPROPERTY()
	double Mass;

	UPROPERTY()
	float Size;

	UPROPERTY()
	double GM;
};

USTRUCT(BlueprintType)
struct FSatelliteStruct
{
	GENERATED_BODY();

	UPROPERTY()
	double Mass;

	UPROPERTY()
	float Size;

	UPROPERTY()
	FOrbitalElements OrbitalElements;
};

USTRUCT(BlueprintType)
struct FSimulationConfigStruct
{
	GENERATED_BODY();

	UPROPERTY()
	int TimeScale;

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
	ACPP_SimulationGameMode();

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	UFUNCTION(BlueprintCallable)
	void InitializeSimulationVariables();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACPP_Planet* Planet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACPP_Satellite*> Satellites;

	FSimulationConfigStruct SimulationConfig;

	double G;

private:
	FSimulationConfigStruct ReadSimulationConfigJson(const FString& SimulationConfigPath);
};
