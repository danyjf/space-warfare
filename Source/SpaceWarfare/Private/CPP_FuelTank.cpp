// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_FuelTank.h"

// Sets default values for this component's properties
UCPP_FuelTank::UCPP_FuelTank()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCPP_FuelTank::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UCPP_FuelTank::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
