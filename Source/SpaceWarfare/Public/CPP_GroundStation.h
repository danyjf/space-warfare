// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStation.generated.h"


// Forward Declaration
//class ACPP_Planet;
class ACPP_Satellite;
//class ACPP_GroundStationManager;


UCLASS()
class SPACEWARFARE_API ACPP_GroundStation : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    class ACPP_GroundStationManager* GroundStationManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int PlayerNumber;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGeographicCoordinates GeographicCoordinates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionFieldOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionHeight;

    UFUNCTION(BlueprintCallable)
    const TArray<ACPP_Satellite*> GetOverpassingSatellites() const { return OverpassingSatellites; }
    UFUNCTION(BlueprintCallable)
    void AddOverpassingSatellite(ACPP_Satellite* Satellite);
    UFUNCTION(BlueprintCallable)
    void RemoveOverpassingSatellite(ACPP_Satellite* Satellite);

    virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TArray<ACPP_Satellite*> OverpassingSatellites;
};
