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
    TMap<FName, class UCPP_SatelliteCommand*> SatelliteCommands;

    UFUNCTION(BlueprintCallable)
    void SendCommandToSatellite(const FName& SatelliteID, UCPP_SatelliteCommand* SatelliteCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const FName& SatelliteID, const FTorqueCommandData& TorqueCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustForDurationCommand(const FName& SatelliteID, const FThrustCommandData& ThrustCommandData, bool bUseLocalCoordinates = false);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustDeactivate(const FName& SatelliteID);

	// Sets default values for this component's properties
	UCPP_SatelliteCommandManager();

private:
    class ACPP_GroundStationManager* GroundStationManager;
};
