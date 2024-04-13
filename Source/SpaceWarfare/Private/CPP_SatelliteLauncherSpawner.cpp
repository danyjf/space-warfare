// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteLauncherSpawner.h"
#include "CPP_Planet.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_SatelliteLauncherSpawner::ACPP_SatelliteLauncherSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_SatelliteLauncherSpawner::BeginPlay()
{
	Super::BeginPlay();
    Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
}

// Called every frame
void ACPP_SatelliteLauncherSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
