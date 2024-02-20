// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_OrbitSpline.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_OrbitSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_OrbitSpline();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class USplineComponent* Spline;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
