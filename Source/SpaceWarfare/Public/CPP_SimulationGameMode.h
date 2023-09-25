// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_SimulationGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_SimulationGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ApplyGravity(TArray<AActor*> actors);

	double GravitationalConstant = 6.6743 * pow(10, -11);

private:
	FVector GravitationalForce(FVector location1, FVector location2, float mass1, float mass2);
};
