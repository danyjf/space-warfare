// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_BotGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_BotGameMode : public ACPP_MultiplayerGameMode
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_GroundStationSpawner> GroundStationSpawnerBlueprint;

	ACPP_BotGameMode();

    // Called after each player logs in
    virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
    // Called when all players have joined the session
    virtual void StartGameplay() override;
};
