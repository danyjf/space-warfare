// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Planet.h"
#include "Universe.h"
#include "CPP_GravityComponent.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_MultiplayerGameMode.h"

#include "Kismet/GameplayStatics.h"

ACPP_Planet::ACPP_Planet()
{
    PrimaryActorTick.bCanEverTick = true;

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComponent->SetSimulatePhysics(true);
    StaticMeshComponent->SetLinearDamping(0.0f);
    StaticMeshComponent->SetAngularDamping(0.0f);
    StaticMeshComponent->SetEnableGravity(false);
    StaticMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
    RootComponent = StaticMeshComponent;

    GravityComponent = CreateDefaultSubobject<UCPP_GravityComponent>(TEXT("Gravity"));

    CameraOrbitableComponent = CreateDefaultSubobject<UCPP_CameraOrbitableComponent>(TEXT("CameraOrbitable"));
    CameraOrbitableComponent->StartOrbitDistance = 15000.0f;
    CameraOrbitableComponent->MaxOrbitDistance = 50000.0f;
    CameraOrbitableComponent->MinOrbitDistance = 7000.0f;
    CameraOrbitableComponent->ZoomSpeed = 300.0f;
}

// Called when the game starts or when spawned
void ACPP_Planet::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
        MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    }
}

// Called every frame
void ACPP_Planet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (HasAuthority() && MultiplayerGameMode->bWaitingForPlayers)
    {
        return;
    }

	SetActorRotation(GetActorRotation() + (RotationSpeed * DeltaTime));
}

void ACPP_Planet::SetRotationAtEpoch(const FDateTime& Epoch)
{
	FRotator EpochRotationAngle = FRotator(0, UUniverse::GetEarthRotationAngle(Epoch.GetJulianDay()), 0);
	SetActorRotation(EpochRotationAngle);
}
