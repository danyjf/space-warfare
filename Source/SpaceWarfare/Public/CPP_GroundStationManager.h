// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationManager.generated.h"


// Forward Declarations
class ACPP_GroundStation;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewSatelliteDetected, FString, SatelliteName);


UCLASS()
class SPACEWARFARE_API ACPP_GroundStationManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    TArray<ACPP_GroundStation*> GroundStations;

    UPROPERTY(BlueprintAssignable)
    FNewSatelliteDetected OnNewSatelliteDetected;

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void SatelliteEnteredOverpassArea(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TMap<FString, FSatelliteStatus> TrackedSatellites;
};
