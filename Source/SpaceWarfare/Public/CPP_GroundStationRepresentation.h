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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionFieldOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionVisualizationHeight;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    UStaticMeshComponent* DetectionConeVisualization;

    UFUNCTION(BlueprintCallable)
    void SetConeColor(FLinearColor Color);

    virtual void OnConstruction(const FTransform& Transform) override;

    ACPP_GroundStationRepresentation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UMaterialInstanceDynamic* DynamicMaterial;
};
