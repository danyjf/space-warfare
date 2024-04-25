// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSatelliteDetected, int, SatelliteID, FSatelliteInfo, SatelliteInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSatelliteDestroyed, int, SatelliteID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFuelLevelUpdatedSignature, int, SatelliteID, float, FuelPercentage);

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCPP_SatelliteCommandManager* SatelliteCommandManager;

    UPROPERTY(Replicated, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;

    UPROPERTY(BlueprintReadOnly)
    TArray<class ACPP_GroundStation*> GroundStations;
    
    UPROPERTY(BlueprintReadOnly)
    TMap<int, ACPP_Satellite*> OverpassingSatellites;

    UPROPERTY(BlueprintReadOnly)
    TMap<int, FSatelliteInfo> TrackedSatellites;

    UPROPERTY(BlueprintReadOnly)
    TMap<FName, class ACPP_OrbitSpline*> AsteroidOrbits;

    UPROPERTY(BlueprintAssignable)
    FNewSatelliteDetected OnNewSatelliteDetected;

    UPROPERTY(BlueprintAssignable)
    FSatelliteDestroyed OnSatelliteDestroyed;

    UPROPERTY(BlueprintAssignable)
    FFuelLevelUpdatedSignature OnFuelLevelUpdated;

    UFUNCTION(BlueprintCallable)
    void PrintOverpassingSatellites();

    UFUNCTION(BlueprintCallable)
    void SatelliteEnteredOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void SatelliteExitedOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void UpdateSatelliteInfo();

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewAsteroidTracked(const FName& AsteroidID, const FVector& Location, const FVector& Velocity);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewSatelliteTracked(const int SatelliteID, int OwnerID, const FString& Label, float Mass, const FVector& Position, const FRotator& Rotation, const FVector& Velocity, const FDateTime& Epoch);

    UFUNCTION(BlueprintCallable, Client, Unreliable)
    void ClientUpdateSatelliteInfo(const int SatelliteID, const FVector& Position, const FRotator& Rotation, const FVector& Velocity, const FDateTime& Epoch);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientSatelliteDestroyed(const int SatelliteID);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientAsteroidDestroyed(const FName& AsteroidID);

    UFUNCTION(BlueprintCallable, Client, Unreliable)
    void ClientUpdateSatelliteFuelLevel(const int SatelliteID, float FuelPercentage);

    UFUNCTION(BlueprintCallable)
    const FSatelliteInfo& GetTrackedSatelliteInfo(const int SatelliteID);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Sets default values for this actor's properties
	ACPP_GroundStationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_Planet* Planet;
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
    bool bInitialized;
    FTimerHandle UpdateSatellitesTimerHandle;
};
