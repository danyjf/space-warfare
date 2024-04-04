// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"
#include "SatelliteCommandDataStructs.h"

#include "CoreMinimal.h"
#include "CPP_Satellite.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_Satellite : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Label;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACPP_Planet* OrbitingPlanet;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<class UCPP_SatelliteCommand*> Commands;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCPP_GravityComponent* GravityComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent* StaticMeshComponent;

    UFUNCTION(BlueprintCallable)
    const int GetSatelliteID() {return SatelliteID;}

    UFUNCTION(BlueprintCallable)
	const FGeographicCoordinates& GetGeographicCoordinates() const;

    UFUNCTION(BlueprintCallable)
    const FSatelliteInfo& GetSatelliteInfo();

    UFUNCTION(BlueprintCallable)
    void AddCommand(UCPP_SatelliteCommand* Command);

    UFUNCTION(BlueprintCallable)
    void PrintGeographicCoordinates();

    UFUNCTION(Client, Reliable)
    void ClientNewSatelliteCommandAdded();

    UFUNCTION(Client, Reliable)
    void ClientSatelliteCommandExecuted();

    UFUNCTION()
    void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    ACPP_Satellite();    

	virtual void Tick(float DeltaTime) override;

    virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    int SatelliteID;
	FGeographicCoordinates GeographicCoordinates;
    FSatelliteInfo SatelliteInfo;
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
};
