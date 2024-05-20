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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class ACPP_Planet* OrbitingPlanet;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<class UCPP_SatelliteCommand*> Commands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MaxTorqueLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int HorizontalTorqueLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int VerticalTorqueLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int RollTorqueLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_OrbitSpline* OrbitSpline;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCPP_GravityComponent* GravityComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent* StaticMeshComponent;

    UFUNCTION(BlueprintCallable)
    const int GetSatelliteID() {return SatelliteID;}

    UFUNCTION(BlueprintCallable)
	const FGeographicCoordinates& GetGeographicCoordinates() const;

    UFUNCTION(BlueprintCallable)
    void AddCommand(UCPP_SatelliteCommand* Command);

    UFUNCTION(BlueprintCallable)
    void PrintGeographicCoordinates();

    UFUNCTION(BlueprintCallable)
    void PrintCommands();

    UFUNCTION()
    void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    ACPP_Satellite();    

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UPROPERTY(ReplicatedUsing=OnRep_SatelliteID)
    int SatelliteID;
	FGeographicCoordinates GeographicCoordinates;
    FSatelliteInfo SatelliteInfo;
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
    class UCPP_SatelliteCommandManager* SatelliteCommandManager;

    /** 
     * Variable used to replicate the state of the satellite to the clients 
     * so that the orbit display can be updated
    */
    UPROPERTY(Replicated)
    FOrbitalState OrbitalState;

    UFUNCTION()
    void OnRep_SatelliteID();
};
