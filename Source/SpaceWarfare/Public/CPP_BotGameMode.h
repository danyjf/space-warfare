// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_SimulationGameMode.h"
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
    virtual void StartGameplay() override;
};
