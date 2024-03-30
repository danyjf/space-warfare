// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSatelliteDetected, FName, UniqueID, FSatelliteInfo, SatelliteInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSatelliteDestroyed, FName, UniqueID);

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    TArray<class ACPP_GroundStation*> GroundStations;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCPP_SatelliteCommandManager* SatelliteCommandManager;

    UPROPERTY(Replicated, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;
    
    UPROPERTY(BlueprintReadOnly)
    TMap<FName, ACPP_Satellite*> OverpassingSatellites;

    UPROPERTY(BlueprintReadOnly)
    TMap<FName, FSatelliteInfo> TrackedSatellites;

    UPROPERTY(BlueprintAssignable)
    FNewSatelliteDetected OnNewSatelliteDetected;

    UPROPERTY(BlueprintAssignable)
    FSatelliteDestroyed OnSatelliteDestroyed;

    UFUNCTION(BlueprintCallable)
    void SatelliteEnteredOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void SatelliteExitedOverpassArea(class ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void UpdateSatelliteInfo();

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewAsteroidTracked(const FName& UniqueID, const FVector& Location, const FVector& Velocity);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientNewSatelliteTracked(const FName& UniqueID, const FSatelliteInfo& SatelliteInfo);

    UFUNCTION(BlueprintCallable, Client, Unreliable)
    void ClientUpdateSatelliteInfo(const FName& UniqueID, const FSatelliteInfo& SatelliteInfo);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientSatelliteDestroyed(const FName& UniqueID);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientAsteroidDestroyed(const FName& UniqueID);

    UFUNCTION(BlueprintCallable)
    void AddGroundStation(class ACPP_GroundStation* GroundStation);

    UFUNCTION(BlueprintCallable)
    void EnableOrbitVisualization(const FName& SatelliteID);

    UFUNCTION(BlueprintCallable)
    void DisableOrbitVisualization(const FName& SatelliteID);

    UFUNCTION(BlueprintCallable)
    const FSatelliteInfo& GetTrackedSatelliteInfo(const FName& SatelliteID);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

    TMap<FName, class ACPP_OrbitSpline*> SatelliteOrbits;
    TMap<FName, class ACPP_OrbitSpline*> AsteroidOrbits;
};
