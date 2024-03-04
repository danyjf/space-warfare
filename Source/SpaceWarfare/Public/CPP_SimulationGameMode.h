// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SimulationConfigStructs.h"

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_GravityManager* GravityManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_GravityManager> GravityManagerBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_GroundStationManager> GroundStationManagerBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_SatelliteLauncher> SatelliteLauncherBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_Satellite> SatelliteBlueprintClass;

    UPROPERTY(BlueprintReadOnly)
	FSimulationConfigStruct SimulationConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeScale;

    UPROPERTY(BlueprintReadOnly)
	FDateTime CurrentEpoch;

	UFUNCTION(BlueprintCallable)
	void InitializeSimulationVariables();

	ACPP_SimulationGameMode();

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

    // Called after each player logs in
    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FDateTime InitialEpoch;
	float ElapsedTime;
    int CurrentPlayerNumber;
};
