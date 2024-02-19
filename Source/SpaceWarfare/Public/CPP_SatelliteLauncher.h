// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_SatelliteLauncher.generated.h"

// Forward Declarations
class ACPP_Satellite;
class ACPP_Planet;
class ACPP_GravityManager;

UCLASS()
class SPACEWARFARE_API ACPP_SatelliteLauncher : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int PlayerNumber;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACPP_Satellite> SatelliteBlueprintClass;
    
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerLaunchSatellite(FOrbitalElements OrbitalElements);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_SatelliteLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    ACPP_GravityManager* GravityManager;
};
