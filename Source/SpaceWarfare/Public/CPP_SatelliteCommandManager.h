// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_SatelliteCommandManager.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_SatelliteCommandManager : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TMap<FName, FSatelliteCommandData> SatelliteCommands;

    UFUNCTION(BlueprintCallable)
    void SendCommandToSatellite(const FName& SatelliteID, const FSatelliteCommandData& SatelliteCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const FName& SatelliteID, const FTorqueCommandData& TorqueCommand);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustForDurationCommand(const FName& SatelliteID, const FThrustCommandData& ThrustCommand, bool bUseLocalCoordinates = false);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustDeactivate(const FName& SatelliteID);

	// Sets default values for this component's properties
	UCPP_SatelliteCommandManager();

private:
    class ACPP_GroundStationManager* GroundStationManager;
};
