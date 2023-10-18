// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_GravityActor.h"
#include "CPP_Planet.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_Planet : public ACPP_GravityActor
{
	GENERATED_BODY()
	
public:
	double GM;
};