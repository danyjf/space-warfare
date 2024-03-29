﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SatelliteCommands.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_SatelliteCommandManager.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_SatelliteCommandManager : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TArray<FSatelliteCommand> SatelliteCommands;

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const FTorqueCommand& TorqueCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustCommand(const FThrustCommand& ThrustCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustForDurationCommand(const FName& SatelliteID, const FThrustForDurationCommand& ThrustCommand);

	// Sets default values for this component's properties
	UCPP_SatelliteCommandManager();

private:
    class ACPP_GroundStationManager* GroundStationManager;
};
