// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_SatelliteLauncher.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_SatelliteLauncher : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGeographicCoordinates GeographicCoordinates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int LaunchCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_Satellite> SatelliteBlueprintClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    USceneComponent* Root;

    UFUNCTION(BlueprintCallable)
    void SetGeographicCoordinates(const FGeographicCoordinates& Value);

    UFUNCTION(BlueprintCallable)
    FVector GetLocationFromHeight(float Height);

    UFUNCTION(BlueprintCallable)
    FVector GetVelocityFromAngle(float Angle, float Value);
    
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerLaunchSatellite(FOrbitalElements OrbitalElements, const FString& Label, TSubclassOf<class ACPP_Satellite> Satellite);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerLaunchSatelliteWithOrbitalState(FOrbitalState OrbitalState, const FString& Label, TSubclassOf<class ACPP_Satellite> Satellite);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_SatelliteLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
};
