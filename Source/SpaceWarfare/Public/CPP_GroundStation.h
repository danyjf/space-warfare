// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStation.generated.h"


// Forward Declaration
class ACPP_Planet;
class ACPP_Satellite;


UCLASS()
class SPACEWARFARE_API ACPP_GroundStation : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGeographicCoordinates GeographicCoordinates;

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void SatelliteEnteredOverpassArea(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

    UFUNCTION(BlueprintCallable)
    const TArray<ACPP_Satellite*> GetOverpassingSatellites() const { return OverpassingSatellites; }
    UFUNCTION(BlueprintCallable)
    void AddOverpassingSatellite(ACPP_Satellite* Satellite);
    UFUNCTION(BlueprintCallable)
    void RemoveOverpassingSatellite(ACPP_Satellite* Satellite);

    UFUNCTION(BlueprintCallable)
    void AddTrackedSatellite(const FString& SatelliteName, const FSatelliteStatus& SatelliteStatus);

	// Sets default values for this actor's properties
	ACPP_GroundStation();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TArray<ACPP_Satellite*> OverpassingSatellites;
    TMap<FString, FSatelliteStatus> TrackedSatellites;
};
