// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityComponent.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCPP_GravityComponent::UCPP_GravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCPP_GravityComponent::BeginPlay()
{
	Super::BeginPlay();

	RigidBody = GetOwner()->GetComponentByClass<UStaticMeshComponent>()->GetBodyInstanceAsyncPhysicsTickHandle();
}

// Called every frame
void UCPP_GravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCPP_GravityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCPP_GravityComponent, GravitationalParameter);
}

void UCPP_GravityComponent::SetMass(double Value)
{
    Mass = Value;
	GetOwner()->GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), Value);
}

void UCPP_GravityComponent::AddGravityForce(const FVector& Force)
{
    GravityForce += Force;
}

void UCPP_GravityComponent::ClearGravityForce()
{
    GravityForce.Set(0.0, 0.0, 0.0);
}
