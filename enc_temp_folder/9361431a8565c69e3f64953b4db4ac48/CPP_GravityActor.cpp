// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityActor.h"
#include "CPP_SimulationGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"


// Sets default values
ACPP_GravityActor::ACPP_GravityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACPP_GravityActor::BeginPlay()
{
	Super::BeginPlay();

	SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	RigidBody = GetComponentByClass<UStaticMeshComponent>()->GetBodyInstanceAsyncPhysicsTickHandle();

	/*if (this->GetName() == "BP_Earth_C_0") 
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), SimulationGameMode->SimulationConfig.Earth.Mass);
	}*/
	if (this->GetName() == "BP_Satellite_C_0")
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), SimulationGameMode->SimulationConfig.Satellites[0].Mass);
		GetComponentByClass<UStaticMeshComponent>()->SetPhysicsLinearVelocity(SimulationGameMode->SimulationConfig.Satellites[0].InitialVelocity);
	}
}

// Called every frame
void ACPP_GravityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called at a fixed DeltaTime to update physics
void ACPP_GravityActor::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
	
	for (ACPP_GravityActor* const Other : SimulationGameMode->GravityActors)
	{
		if (this == Other)
		{
			continue;
		}

		FVector GravityForce = GetGravityForce(Other);

		RigidBody->AddForce(GravityForce);
	}
}

FVector ACPP_GravityActor::GetGravityForce(ACPP_GravityActor* Other)
{
	FVector MyLocation = RigidBody->X();
	FVector OtherLocation = Other->RigidBody->X();
	float MyMass = RigidBody->M();
	float OtherMass = Other->RigidBody->M();

	FVector Direction = OtherLocation - MyLocation;

	double Force = (SimulationGameMode->G * MyMass * OtherMass) / Direction.SquaredLength();

	Direction.Normalize();

	return Force * Direction;
}

void ACPP_GravityActor::SetMass(double Mass)
{
	GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), Mass);
}

void ACPP_GravityActor::SetLocation(FVector Location)
{

}

void ACPP_GravityActor::SetInitialVelocity(FVector InitialVelocity)
{

}