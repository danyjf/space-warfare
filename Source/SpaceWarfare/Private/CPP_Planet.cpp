// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"


void ACPP_Planet::Initialize(FString aName, double Mass, float Size, double aGM, double aRotationSpeed)
{
	Super::Initialize(aName, Mass, Size, FVector(0.0f), FVector(0.0f));
	GM = aGM;
	RotationSpeed = FRotator(0, -aRotationSpeed, 0);
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}
