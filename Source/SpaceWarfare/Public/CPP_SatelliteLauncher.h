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
    int PlayerNumber;
    
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerLaunchSatellite(FOrbitalElements OrbitalElements);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_SatelliteLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
