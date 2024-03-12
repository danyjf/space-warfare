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
    int LaunchCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_Satellite> SatelliteBlueprintClass;
    
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerLaunchSatellite(FOrbitalElements OrbitalElements, float Size, float Mass, const FString& Label);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_SatelliteLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_SimulationGameMode* SimulationGameMode;
};
