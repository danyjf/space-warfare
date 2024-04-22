// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CPP_GameState.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    bool bWaitingForPlayers;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	FDateTime CurrentEpoch;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int TimeScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<int, class ACPP_Satellite*> AllSatellites;

	ACPP_GameState();

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
