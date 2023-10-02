// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_SimulationGameMode.generated.h"


// Forward Declarations
class ACPP_GravityActor;


USTRUCT(BlueprintType)
struct GravityBodyStruct
{
	GENERATED_BODY()

	UPROPERTY()
	double Mass;

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector InitialVelocity;
};

USTRUCT(BlueprintType)
struct SimulationConfigStruct
{
	GENERATED_BODY()
	
	UPROPERTY()
	int Time;

	UPROPERTY()
	double GravitationalConstant;

	UPROPERTY()
	GravityBodyStruct Earth;

	UPROPERTY()
	TArray<GravityBodyStruct> Satellites;
};


/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_SimulationGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACPP_GravityActor*> GravityActors;
};
