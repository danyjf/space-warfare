// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GameState.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_Satellite.h"
#include "CPP_Asteroid.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACPP_GameState::ACPP_GameState()
{
    bAsyncPhysicsTickEnabled = true;

    bWaitingForPlayers = true;
}

void ACPP_GameState::BeginPlay()
{
    Super::BeginPlay();
}

// Called at a fixed DeltaTime to update physics
void ACPP_GameState::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

    if (bWaitingForPlayers)
    {
        return;
    }

	// Calculate current time
	float ScaledDeltaTime = DeltaTime * TimeScale;
    FTimespan ModifyTime = FTimespan::FromSeconds(ScaledDeltaTime);
    CurrentEpoch += ModifyTime;
}

void ACPP_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACPP_GameState, CurrentEpoch);
    DOREPLIFETIME(ACPP_GameState, bWaitingForPlayers);
    DOREPLIFETIME(ACPP_GameState, TimeScale);
}
