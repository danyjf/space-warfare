// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_SimulationGameMode.generated.h"


// Forward Declarations
class ACPP_GravityActor;


USTRUCT(BlueprintType)
struct FGravityBodyStruct
{
	GENERATED_BODY()

	UPROPERTY()
	double Mass;

	UPROPERTY()
	float Size;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector InitialVelocity;
};

USTRUCT(BlueprintType)
struct FSimulationConfigStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	int Time;

	UPROPERTY()
	double GravitationalConstant;

	UPROPERTY()
	FGravityBodyStruct Earth;

	UPROPERTY()
	TArray<FGravityBodyStruct> Satellites;
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

	UFUNCTION(BlueprintCallable)
	void InitializeSimulationVariables();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACPP_GravityActor*> GravityActors;

	FSimulationConfigStruct SimulationConfig;

	double G;

private:
	FSimulationConfigStruct ReadSimulationConfigJson(const FString& SimulationConfigPath);
};
