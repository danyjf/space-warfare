// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_Asteroid.h"
#include "CPP_GravityComponent.h"

// Sets default values
ACPP_Asteroid::ACPP_Asteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetLinearDamping(0.0f);
    StaticMeshComponent->SetAngularDamping(0.0f);
    StaticMeshComponent->SetEnableGravity(false);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    RootComponent = StaticMeshComponent;

    GravityComponent = CreateDefaultSubobject<UCPP_GravityComponent>(TEXT("Gravity"));
}

// Called when the game starts or when spawned
void ACPP_Asteroid::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_Asteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}