// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"
#include "Gravity.h"


void ACPP_Planet::Initialize(FString aName, double Mass, float Size, double aGM, double aRotationSpeed, FString Epoch)
{
	Super::Initialize(aName, Mass, Size, FVector(0.0f), FVector(0.0f));
	GM = aGM;
	RotationSpeed = FRotator(0, -aRotationSpeed, 0);

	FDateTime OutEpoch;
	FDateTime::ParseIso8601(*Epoch, OutEpoch);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), OutEpoch.GetJulianDay());
	FRotator EpochRotationAngle = FRotator(0, -UGravity::GetEarthRotationAngle(OutEpoch.GetJulianDay()), 0);
	SetActorRotation(EpochRotationAngle);
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}
