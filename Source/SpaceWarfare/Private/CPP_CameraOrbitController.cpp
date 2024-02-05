// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CameraOrbitController.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_Planet.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


// Called when the game starts or when spawned
void ACPP_CameraOrbitController::BeginPlay()
{
	Super::BeginPlay();

    OrbitingActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass());
    CameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(OrbitingActor->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SpringArmComponent = Cast<USpringArmComponent>(PlayerPawn->GetComponentByClass(USpringArmComponent::StaticClass()));

    PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
}

// Called every frame
void ACPP_CameraOrbitController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    
}
