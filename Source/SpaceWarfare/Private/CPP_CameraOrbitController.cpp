// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_CameraOrbitController.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_Planet.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GroundStationSpawner.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ACPP_CameraOrbitController::ACPP_CameraOrbitController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PlayerID = -1;
    bHasNecessaryReplicatedVariables = false;
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

void ACPP_CameraOrbitController::SetupInputComponent() 
{
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(InputMapping, 0);

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
    Input->BindAction(SelectAction, ETriggerEvent::Triggered, this, &ACPP_CameraOrbitController::MouseSelect);
    Input->BindAction(DragAction, ETriggerEvent::Triggered, this, &ACPP_CameraOrbitController::MouseDrag);
}

// Called every frame
void ACPP_CameraOrbitController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Check if this player controller is on the client or (is on the server and is controlled by the server)
    if (!bHasNecessaryReplicatedVariables && (!HasAuthority() || (HasAuthority() && IsLocalPlayerController())))
    {
        if (PlayerID != -1 && UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass()))
        {
            ServerPlayerFinishedJoiningSession();
            bHasNecessaryReplicatedVariables = true;
        }
    }

    PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
}

void ACPP_CameraOrbitController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_CameraOrbitController, Currency, COND_OwnerOnly);
    DOREPLIFETIME(ACPP_CameraOrbitController, PlayerID);
    DOREPLIFETIME(ACPP_CameraOrbitController, PlayerStatus);
}

void ACPP_CameraOrbitController::OnRep_Currency()
{
    OnCurrencyUpdated.Broadcast(Currency);
}

void ACPP_CameraOrbitController::ServerPlayerFinishedJoiningSession_Implementation()
{
    bHasNecessaryReplicatedVariables = true;
}

void ACPP_CameraOrbitController::ServerPlayerFinishedPlacingGroundStations_Implementation(bool bFinished)
{
    bFinishedPlacingGroundStations = bFinished;
}

void ACPP_CameraOrbitController::ClientAllPlayersFinishedPlacingGroundStations_Implementation()
{
    OnAllPlayersFinishedPlacingGroundStations.Broadcast();
}

void ACPP_CameraOrbitController::SpendCurrency(int Amount)
{
    Currency -= Amount;
    if (IsLocalPlayerController())
    {
        OnCurrencyUpdated.Broadcast(Currency);
    }
}

void ACPP_CameraOrbitController::MouseSelect(const FInputActionValue& Value)
{
    FHitResult HitResult;
    GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
    AActor* HitActor = HitResult.GetActor();

    switch (PlayerStatus)
    {
    case EPlayerStatus::PLACING_GROUND_STATIONS:
    {
        ACPP_Planet* Planet = Cast<ACPP_Planet>(HitActor);
        ACPP_GroundStationSpawner* GroundStationSpawner = Cast<ACPP_GroundStationSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationSpawner::StaticClass()));
        if (!IsValid(Planet) || !IsValid(GroundStationSpawner))
        {
            return;
        }

        if (GroundStationSpawner->bIsChoosingLocation)
        {
            GroundStationSpawner->UpdateGroundStationRepresentationLocation(HitResult.Location);
        }
        else
        {
            GroundStationSpawner->SpawnGroundStationRepresentation(HitResult.Location);
        }

        break;
    }
    case EPlayerStatus::GROUND_STATION_CONTROL:
    {
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

        break;
    }
    }
}

void ACPP_CameraOrbitController::MouseDrag(const FInputActionValue& Value)
{
    PlayerPawn->AddControllerYawInput(Value.Get<FInputActionValue::Axis2D>().X);
    PlayerPawn->AddControllerPitchInput(-Value.Get<FInputActionValue::Axis2D>().Y);
}
