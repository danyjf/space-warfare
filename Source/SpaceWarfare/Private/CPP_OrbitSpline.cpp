// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_OrbitSpline.h"

#include "Components/SplineComponent.h"

// Sets default values
ACPP_OrbitSpline::ACPP_OrbitSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    Spline->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ACPP_OrbitSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_OrbitSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
