// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_GroundStationBase.h"
#include "CPP_GroundStationRepresentation.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_GroundStationRepresentation : public ACPP_GroundStationBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Cost;

    UFUNCTION(BlueprintCallable)
    void UpdateCost();
};
