// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_GravityComponent.h"


ACPP_Planet::ACPP_Planet()
{
    PrimaryActorTick.bCanEverTick = true;

    //StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    //StaticMeshComponent->SetSimulatePhysics(true);
    //StaticMeshComponent->SetLinearDamping(0.0f);
    //StaticMeshComponent->SetAngularDamping(0.0f);
    //StaticMeshComponent->SetEnableGravity(false);
    //StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    //RootComponent = StaticMeshComponent;
}

// Called when the game starts or when spawned
void ACPP_Planet::BeginPlay()
{
	Super::BeginPlay();
	
    MyGravityComponent = Cast<UCPP_GravityComponent>(GetComponentByClass<UCPP_GravityComponent>());
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}

void ACPP_Planet::SetRotationAtEpoch(const FDateTime& Epoch)
{
	FRotator EpochRotationAngle = FRotator(0, UUniverse::GetEarthRotationAngle(Epoch.GetJulianDay()), 0);
	SetActorRotation(EpochRotationAngle);
}
