// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_SatelliteCommandManager.generated.h"

USTRUCT(BlueprintType)
struct FSatelliteCommandList
{
	GENERATED_BODY()

    UPROPERTY()
    TArray<class UCPP_SatelliteCommand*> CommandList;
};

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_SatelliteCommandManager : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TMap<int, FSatelliteCommandList> PendingSatelliteCommands;

    UFUNCTION(BlueprintCallable)
    void SendCommandToSatellite(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommandData);

    UFUNCTION(BlueprintCallable)
    void SendPendingCommandsToSatellite(const int SatelliteID);

    UFUNCTION(BlueprintCallable)
    void StoreSatelliteCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand);

    UFUNCTION(Client, Reliable)
    void ClientRemovePendingSatelliteCommand();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const int SatelliteID, const FTorqueCommandData& TorqueCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustCommand(const int SatelliteID, const FThrustCommandData& ThrustCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustDeactivate(const int SatelliteID);

	// Sets default values for this component's properties
	UCPP_SatelliteCommandManager();

private:
    class ACPP_GroundStationManager* GroundStationManager;
};
