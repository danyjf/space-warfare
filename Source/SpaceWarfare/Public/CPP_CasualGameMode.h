// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_CasualGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_CasualGameMode : public ACPP_MultiplayerGameMode
{
	GENERATED_BODY()
	
public:
	ACPP_CasualGameMode();

protected:
    // Called when all players have joined the session
    virtual void StartGameplay() override;
};
