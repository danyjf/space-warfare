// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_Thruster.h"
#include "CPP_Satellite.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCPP_Thruster::UCPP_Thruster()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UCPP_Thruster::BeginPlay()
{
	Super::BeginPlay();

    StaticMeshComponent = Cast<ACPP_Satellite>(GetOwner())->StaticMeshComponent;
}

// Called every frame
void UCPP_Thruster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bThrusterIsActive && ThrusterTimer <= ThrusterDuration)
    {
        StaticMeshComponent->AddForce(ThrusterStrength * UKismetMathLibrary::TransformDirection(GetOwner()->GetTransform(), ThrusterDirection));
        ThrusterTimer += DeltaTime;
    }
}

void UCPP_Thruster::SetThrusterDirectionInLocalCoordinates(FVector Direction)
{
    ThrusterDirection = Direction;
}

void UCPP_Thruster::SetThrusterDirectionInECICoordinates(FVector Direction)
{
    ThrusterDirection = UKismetMathLibrary::InverseTransformDirection(GetOwner()->GetTransform(), Direction);
    ThrusterDirection.Normalize();
}

/**
 * When duration is 0 it stays active until told to deactivate
*/
void UCPP_Thruster::ActivateThruster(float Strength, float Duration)
{
    if (ThrusterStrength > MaxThrusterStrength)
    {
        return;
    }

    ThrusterDuration = Duration;
    if (UKismetMathLibrary::NearlyEqual_FloatFloat(ThrusterDuration, 0.0f))
    {
        ThrusterDuration = TNumericLimits<int32>::Max();
    }
    ThrusterStrength = Strength;
    bThrusterIsActive = true;
    ThrusterTimer = 0.0f;
}

void UCPP_Thruster::DeactivateThruster()
{
    bThrusterIsActive = false;
    ThrusterDuration = 0.0f;
    ThrusterTimer = 0.0f;
}
