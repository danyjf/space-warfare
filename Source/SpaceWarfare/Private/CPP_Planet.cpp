// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"
#include "Universe.h"


ACPP_Planet::ACPP_Planet()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}

void ACPP_Planet::SetRotationAtEpoch(const FDateTime& Epoch)
{
	FRotator EpochRotationAngle = FRotator(0, UUniverse::GetEarthRotationAngle(Epoch.GetJulianDay()), 0);
	SetActorRotation(EpochRotationAngle);
}
