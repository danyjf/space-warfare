// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CameraOrbitController.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_Planet.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACPP_CameraOrbitController::ACPP_CameraOrbitController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    InputMode = EInputMode::GROUNDSTATIONINPUT;
    ClickTimer = 0.0f;
    ClickThreshold = 0.1f;
}

// Called when the game starts or when spawned
void ACPP_CameraOrbitController::BeginPlay()
{
	Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SpringArmComponent = Cast<USpringArmComponent>(PlayerPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
}

// Called every frame
void ACPP_CameraOrbitController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!OrbitingActor)
    {
        OrbitingActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass());
        if (OrbitingActor)
        {
            CameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(OrbitingActor->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
            PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
        }
        return;
    }

    switch (InputMode)
    {
        case EInputMode::GODMODEINPUT:
            ClickTimer += DeltaTime;

        case EInputMode::GROUNDSTATIONINPUT:
            PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
            break;
    
        default:
            break;
    }
}

void ACPP_CameraOrbitController::HandleLeftMouseButtonPress()
{
    if (InputMode != EInputMode::GODMODEINPUT)
    {
        return;
    }

    ClickTimer = 0.0f;
}

void ACPP_CameraOrbitController::HandleLeftMouseButtonRelease()
{
    if (InputMode != EInputMode::GODMODEINPUT)
    {
        return;
    }

    if (ClickTimer > ClickThreshold)
    {
        return;
    }
    
    FHitResult HitResult;
    GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

    AActor* HitActor = HitResult.GetActor();
    if (!IsValid(HitActor) || HitActor == OrbitingActor)
    {
        return;
    }

    UCPP_CameraOrbitableComponent* HitCameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(HitActor->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
    if (IsValid(HitCameraOrbitableComponent))
    {
        CameraOrbitableComponent = HitCameraOrbitableComponent;
        SpringArmComponent->TargetArmLength = CameraOrbitableComponent->StartOrbitDistance;
        OrbitingActor = HitActor;
    }
}
