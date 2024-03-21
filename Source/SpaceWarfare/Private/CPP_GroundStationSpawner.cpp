// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationSpawner.h"
#include "CPP_GroundStation.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACPP_GroundStationSpawner::ACPP_GroundStationSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACPP_GroundStationSpawner::BeginPlay()
{
	Super::BeginPlay();

    UKismetSystemLibrary::PrintString(GetWorld(), "GroundStationSpawner");
}

// Called every frame
void ACPP_GroundStationSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
