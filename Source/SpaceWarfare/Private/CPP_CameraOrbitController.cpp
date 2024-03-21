// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CameraOrbitController.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_Planet.h"
#include "CPP_GroundStationManager.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACPP_CameraOrbitController::ACPP_CameraOrbitController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PlayerID = -1;
    ClickTimer = 0.0f;
    ClickThreshold = 0.1f;
    Ready = false;
    PlayerStatus = EPlayerStatus::WAITING;
}

// Called when the game starts or when spawned
void ACPP_CameraOrbitController::BeginPlay()
{
	Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SpringArmComponent = Cast<USpringArmComponent>(PlayerPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
    OrbitingActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass());
    CameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(OrbitingActor->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
    PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
}

// Called every frame
void ACPP_CameraOrbitController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Check if this player controller is on the client or (is on the server and is controlled by the server)
    if (!Ready && (!HasAuthority() || (HasAuthority() && IsLocalPlayerController())))
    {
        if (PlayerID != -1 && UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass()))
        {
            ServerPlayerReady();
            Ready = true;
        }
    }

    ClickTimer += DeltaTime;
    PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
}

void ACPP_CameraOrbitController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_CameraOrbitController, Currency, COND_OwnerOnly);
    DOREPLIFETIME(ACPP_CameraOrbitController, PlayerID);
    DOREPLIFETIME(ACPP_CameraOrbitController, PlayerState);
}

void ACPP_CameraOrbitController::OnRep_Currency()
{
    OnCurrencyUpdated.Broadcast(Currency);
}

void ACPP_CameraOrbitController::ServerPlayerReady_Implementation()
{
    Ready = true;
}

void ACPP_CameraOrbitController::SpendCurrency(int Amount)
{
    Currency -= Amount;
    if (IsLocalPlayerController())
    {
        OnCurrencyUpdated.Broadcast(Currency);
    }
}

void ACPP_CameraOrbitController::HandleLeftMouseButtonPress()
{
    ClickTimer = 0.0f;
}

void ACPP_CameraOrbitController::HandleLeftMouseButtonRelease()
{
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
