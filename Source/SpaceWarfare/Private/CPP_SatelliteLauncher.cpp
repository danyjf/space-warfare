// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SatelliteLauncher.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACPP_SatelliteLauncher::ACPP_SatelliteLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PlayerNumber = 0;
}

// Called when the game starts or when spawned
void ACPP_SatelliteLauncher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_SatelliteLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_SatelliteLauncher::ServerLaunchSatellite_Implementation(FOrbitalElements OrbitalElements)
{
    UKismetSystemLibrary::PrintString(this, "Launching Satellite");
}
