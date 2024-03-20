// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SimulationConfigStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CPP_MultiplayerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_MultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bWaitingForPlayers;

    /** Number of players to be used when not starting the game from the main menu */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int DefaultNumberOfPlayers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_GravityManager* GravityManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_GravityManager> GravityManagerBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_GroundStationManager> GroundStationManagerBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_SatelliteLauncher> SatelliteLauncherBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_Satellite> SatelliteBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int StartingCurrency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TimeScale;

    UPROPERTY(BlueprintReadOnly)
	FDateTime CurrentEpoch;

	UFUNCTION(BlueprintCallable)
	void InitializeSimulation(const FSimulationConfig& SimulationConfig);

	UFUNCTION(BlueprintCallable)
	void InitializeSatellites(TArray<FSatelliteStruct>& SatellitesConfigs);

	ACPP_MultiplayerGameMode();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

    // Called after each player logs in
    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // Called when all players have joined the session
    virtual void StartGameplay();

private:
	FDateTime InitialEpoch;
	float ElapsedTime;
    int CurrentPlayerID;
    class UCPP_GameInstance* GameInstance;

    template <class T>
    void ShuffleArray(T& InArray);
};
