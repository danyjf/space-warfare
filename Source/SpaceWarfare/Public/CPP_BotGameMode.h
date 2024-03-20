// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_BotGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_BotGameMode : public ACPP_SimulationGameMode
{
	GENERATED_BODY()

public:
	ACPP_BotGameMode();

protected:
    // Called when all players have joined the session
    virtual void StartGameplay() override;
};
