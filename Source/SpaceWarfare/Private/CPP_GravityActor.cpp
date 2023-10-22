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

	TotalForces = FVector(0.0);
}

// Called when the game starts or when spawned
void ACPP_GravityActor::BeginPlay()
{
	Super::BeginPlay();

	SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	RigidBody = GetComponentByClass<UStaticMeshComponent>()->GetBodyInstanceAsyncPhysicsTickHandle();
}

// Called every frame
void ACPP_GravityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GravityActor::Initialize(FString aName, double Mass, float Size, FVector Location, FVector InitialVelocity)
{
	Name = aName;
	GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), Mass);
	SetActorScale3D(FVector(Size));
	SetActorLocation(Location);
	Velocity = InitialVelocity;
}

void ACPP_GravityActor::AddForce(FVector Force)
{
	TotalForces += Force;
}

void ACPP_GravityActor::ResetForces()
{
	TotalForces.Set(0.0, 0.0, 0.0);
}

void ACPP_GravityActor::UpdateVelocity(float DeltaTime)
{
	FVector Acceleration = TotalForces / RigidBody->M();
	Velocity += Acceleration * DeltaTime;
}

void ACPP_GravityActor::UpdateLocation(float DeltaTime)
{
	RigidBody->SetX(RigidBody->X() + Velocity * DeltaTime);
}
