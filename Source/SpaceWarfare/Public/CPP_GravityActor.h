// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GravityActor.generated.h"

// Forward Declarations
class ACPP_SimulationGameMode;

UCLASS()
class SPACEWARFARE_API ACPP_GravityActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_GravityActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	ACPP_SimulationGameMode* SimulationGameMode;
	double G = 66743 * 9;

	FVector Gravity(ACPP_GravityActor* Actor);
};
