// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_SatelliteLauncherSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSatelliteLauncherRepresentationUpdate);

UCLASS()
class SPACEWARFARE_API ACPP_SatelliteLauncherSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsChoosingLocation;

    UPROPERTY()
    class ACPP_SatelliteLauncher* SatelliteLauncherRepresentation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_SatelliteLauncher> SatelliteLauncherBlueprint;

    UPROPERTY(BlueprintAssignable)
    FSatelliteLauncherRepresentationUpdate OnUpdateSatelliteLauncherRepresentation;

    UFUNCTION()
    void SpawnSatelliteLauncherRepresentation(const FVector& Location);

    UFUNCTION()
    void UpdateSatelliteLauncherLocation(const FVector& Location);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSpawnSatelliteLauncher(const FVector& Location);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_SatelliteLauncherSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
