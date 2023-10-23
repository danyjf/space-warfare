// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"


void ACPP_Planet::Initialize(FString aName, double Mass, float Size, double aGM)
{
	Super::Initialize(aName, Mass, Size, FVector(0.0f), FVector(0.0f));
	GM = aGM;
}
