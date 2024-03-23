// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationBase.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationBase : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGeographicCoordinates GeographicCoordinates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionFieldOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionVisualizationHeight;

    UFUNCTION(BlueprintCallable)
    void SetGeographicCoordinates(const FGeographicCoordinates& Value);

    virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStationBase();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* DetectionCone;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* DetectionConeVisualization;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
