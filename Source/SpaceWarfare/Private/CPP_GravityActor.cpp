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

	if (this->GetName() == "BP_Earth_C_0") 
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), 1000);
	}
	else if (this->GetName() == "BP_Satellite_C_0")
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), 10);
		GetComponentByClass<UStaticMeshComponent>()->SetPhysicsLinearVelocity(FVector(506.65949 * 2, 0.0, 0.0));
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

	FBodyInstanceAsyncPhysicsTickHandle MyBodyInstance = GetComponentByClass<UStaticMeshComponent>()->GetBodyInstanceAsyncPhysicsTickHandle();
	FTransform MyTransform = FTransform(MyBodyInstance->R(), MyBodyInstance->X());
	for (int i = 0; i < SimulationGameMode->GravityActors.Num(); i++)
	{
		if (this == SimulationGameMode->GravityActors[i])
			continue;

		FBodyInstanceAsyncPhysicsTickHandle OtherBodyInstance = SimulationGameMode->GravityActors[i]->GetComponentByClass<UStaticMeshComponent>()->GetBodyInstanceAsyncPhysicsTickHandle();
		FTransform OtherTransform = FTransform(OtherBodyInstance->R(), OtherBodyInstance->X());

		FVector MyLocation = MyTransform.GetLocation();
		FVector OtherLocation = OtherTransform.GetLocation();
		float MyMass = MyBodyInstance->M();
		float OtherMass = OtherBodyInstance->M();
		
		FVector Direction = OtherLocation - MyLocation;

		double Force = (G * MyMass * OtherMass) / Direction.SquaredLength();

		Direction.Normalize();

		MyBodyInstance->AddForce(Force * Direction);
	}
}
