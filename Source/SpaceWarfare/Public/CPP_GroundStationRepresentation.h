// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_GroundStationBase.h"
#include "CPP_GroundStationRepresentation.generated.h"

/**
 * An empty version of the ground station that is essentially used for
 * visualization on the client side of where the ground station will be 
 * spawned on the server
 */
UCLASS()
class SPACEWARFARE_API ACPP_GroundStationRepresentation : public ACPP_GroundStationBase
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetConeColor(FLinearColor Color);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UMaterialInstanceDynamic* DynamicMaterial;
};
