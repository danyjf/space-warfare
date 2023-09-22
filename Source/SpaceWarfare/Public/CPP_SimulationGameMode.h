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
	void ApplyGravity(TArray<AActor*> actors, TArray<float> masses, float gravitationalConstant);

private:
	FVector GravitationalForce(FVector location1, FVector location2, float mass1, float mass2, float gravitationalConstant);
};
