// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Thruster.h"
#include "CPP_GravityComponent.h"

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

    GravityComponent = Cast<UCPP_GravityComponent>(GetOwner()->FindComponentByClass(UCPP_GravityComponent::StaticClass()));
}


// Called every frame
void UCPP_Thruster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bThrusterIsActive)
    {
        GravityComponent->AddGravityForce(ThrusterStrength * GetOwner()->GetActorForwardVector());
    }
}
