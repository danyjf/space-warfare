// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStation.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_GroundStation::ACPP_GroundStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_GroundStation::BeginPlay()
{
	Super::BeginPlay();

    // Hide enemy ground stations on listen server
    if (HasAuthority() && UGameplayStatics::GetPlayerController(GetWorld(), 0) != GetOwner())
    {
        SetActorHiddenInGame(true);
    }
    else if (!HasAuthority())
    {
        SetActorHiddenInGame(false);
    }
}

// Called every frame
void ACPP_GroundStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
