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

    UPROPERTY(BlueprintReadOnly)
    TArray<class UCPP_SatelliteCommand*> CommandList;
};

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_SatelliteCommandManager : public UActorComponent
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TMap<int, FSatelliteCommandList> PendingSatelliteCommands;

    /**
     * This variable is used to store the satellite commands on the client
     * side so that the UI can have access to them.
     * 
     * It's being done this way because initially, the idea was for the client
     * to not have access to the satellites themselves
    */
    UPROPERTY(BlueprintReadOnly)
    TMap<int, FSatelliteCommandList> SatelliteCommands;

    UFUNCTION()
    void HandleNewCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand);

    UFUNCTION(BlueprintCallable)
    void SendPendingCommandsToSatellite(const int SatelliteID);

    UFUNCTION(BlueprintCallable)
    void PrintPendingSatelliteCommands();

    UFUNCTION(BlueprintCallable)
    void StorePendingSatelliteCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand);

    UFUNCTION(BlueprintCallable)
    void StoreSatelliteCommand(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommand);

    UFUNCTION(Client, Reliable)
    void ClientSendPendingSatelliteCommands(const int SatelliteID);

    UFUNCTION(Client, Reliable)
    void ClientSatelliteExecutedCommand(const int SatelliteID);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteTorqueCommand(const int SatelliteID, const FTorqueCommandData& TorqueCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustCommand(const int SatelliteID, const FThrustCommandData& ThrustCommandData);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSatelliteThrustDeactivate(const int SatelliteID);

	// Sets default values for this component's properties
	UCPP_SatelliteCommandManager();

protected:
    virtual void BeginPlay() override;

private:
    class ACPP_GroundStationManager* GroundStationManager;
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;

    void SendCommandToSatellite(const int SatelliteID, UCPP_SatelliteCommand* SatelliteCommandData);
};
