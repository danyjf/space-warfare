// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_FuelTank.h"
#include "CPP_Satellite.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_GroundStationManager.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCPP_FuelTank::UCPP_FuelTank()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

    MaxFuel = 100.0f;
}

// Called when the game starts
void UCPP_FuelTank::BeginPlay()
{
	Super::BeginPlay();

	MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    Satellite = Cast<ACPP_Satellite>(GetOwner());
    FuelLevel = MaxFuel;
}

// Called every frame
void UCPP_FuelTank::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UCPP_FuelTank::GetFuelLevel()
{
    return FuelLevel;
}

/**
 * Spends fuel from the fuel tank.
 * @param Value The amount of fuel to remove from the tank.
 * @return  A boolean representing whether the fuel tank had enough fuel to spend.
 *          If it didn't have enough, then the bool returns false and the fuel is not spent.
*/
bool UCPP_FuelTank::SpendFuel(float Value)
{
    if (FuelLevel - Value < 0.0f)
    {
        return false;
    }

    FuelLevel -= Value;
    MultiplayerGameMode->GetGroundStationManagers()[Satellite->OwnerPlayerID]->ClientUpdateSatelliteFuelLevel(Satellite->GetSatelliteID(), FuelLevel / MaxFuel);
    return true;
}
