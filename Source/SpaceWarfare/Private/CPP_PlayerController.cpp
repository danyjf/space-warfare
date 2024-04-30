// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_PlayerController.h"
#include "CPP_CameraOrbitableComponent.h"
#include "CPP_Planet.h"
#include "CPP_GroundStationManager.h"
#include "CPP_GroundStationSpawner.h"
#include "CPP_SatelliteLauncherSpawner.h"

#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ACPP_PlayerController::ACPP_PlayerController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PlayerID = -1;
    bHasNecessaryReplicatedVariables = false;
    bMouseInputEnabled = true;
    PlayerStatus = EPlayerStatus::WAITING;
}

// Called when the game starts or when spawned
void ACPP_PlayerController::BeginPlay()
{
	Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SpringArmComponent = Cast<USpringArmComponent>(PlayerPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
    OrbitingActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass());
    PlayerPawn->AttachToActor(OrbitingActor, FAttachmentTransformRules::KeepRelativeTransform);
    CameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(OrbitingActor->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
    PlayerPawn->SetActorLocation(OrbitingActor->GetActorLocation());
}

void ACPP_PlayerController::SetupInputComponent() 
{
    Super::SetupInputComponent();

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(InputMapping, 0);

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
    Input->BindAction(SelectAction, ETriggerEvent::Triggered, this, &ACPP_PlayerController::MouseSelect);
    Input->BindAction(DragAction, ETriggerEvent::Triggered, this, &ACPP_PlayerController::MouseDrag);
}

// Called every frame
void ACPP_PlayerController::Tick(float DeltaTime)
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
}

void ACPP_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_PlayerController, Currency, COND_OwnerOnly);
    DOREPLIFETIME(ACPP_PlayerController, PlayerID);
    DOREPLIFETIME(ACPP_PlayerController, PlayerStatus);
}

void ACPP_PlayerController::OnRep_Currency()
{
    OnCurrencyUpdated.Broadcast(Currency);
}

void ACPP_PlayerController::ServerPlayerFinishedJoiningSession_Implementation()
{
    bHasNecessaryReplicatedVariables = true;
}

void ACPP_PlayerController::ServerPlayerFinishedPlacingGroundStations_Implementation(bool bFinished)
{
    bFinishedPlacingGroundStations = bFinished;
}

void ACPP_PlayerController::ClientAllPlayersFinishedPlacingGroundStations_Implementation()
{
    OnAllPlayersFinishedPlacingGroundStations.Broadcast();
}

void ACPP_PlayerController::SpendCurrency(int Amount)
{
    Currency -= Amount;
    if (IsLocalPlayerController())
    {
        OnCurrencyUpdated.Broadcast(Currency);
    }
}

void ACPP_PlayerController::MouseSelect(const FInputActionValue& Value)
{
    if (!bMouseInputEnabled)
    {
        return;
    }

    FHitResult HitResult;
    GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
    AActor* HitActor = HitResult.GetActor();

    switch (PlayerStatus)
    {
    case EPlayerStatus::PLACING_SATELLITE_LAUNCHER:
    {
        ACPP_Planet* Planet = Cast<ACPP_Planet>(HitActor);
        ACPP_SatelliteLauncherSpawner* SatelliteLauncherSpawner = Cast<ACPP_SatelliteLauncherSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_SatelliteLauncherSpawner::StaticClass()));
        if (!IsValid(Planet) || !IsValid(SatelliteLauncherSpawner) || !SatelliteLauncherSpawner->bCanSpawnSatelliteLauncher)
        {
            return;
        }

        if (SatelliteLauncherSpawner->bIsChoosingLocation)
        {
            SatelliteLauncherSpawner->UpdateSatelliteLauncherLocation(HitResult.Location);
        }
        else
        {
            SatelliteLauncherSpawner->SpawnSatelliteLauncherRepresentation(HitResult.Location);
        }

        break;
    }
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

        SetOrbitingActor(HitActor);

        break;
    }
    }
}

void ACPP_PlayerController::MouseDrag(const FInputActionValue& Value)
{
    if (!bMouseInputEnabled)
    {
        return;
    }

    PlayerPawn->AddControllerYawInput(Value.Get<FInputActionValue::Axis2D>().X);
    PlayerPawn->AddControllerPitchInput(-Value.Get<FInputActionValue::Axis2D>().Y);
}

void ACPP_PlayerController::SetOrbitingActor(AActor* ActorToOrbit)
{
    UCPP_CameraOrbitableComponent* OrbitCameraOrbitableComponent = Cast<UCPP_CameraOrbitableComponent>(ActorToOrbit->GetComponentByClass(UCPP_CameraOrbitableComponent::StaticClass()));
    if (IsValid(OrbitCameraOrbitableComponent))
    {
        OrbitingActor = ActorToOrbit;
        PlayerPawn->AttachToActor(OrbitingActor, FAttachmentTransformRules::KeepRelativeTransform);
        CameraOrbitableComponent = OrbitCameraOrbitableComponent;
        SpringArmComponent->TargetArmLength = CameraOrbitableComponent->StartOrbitDistance;
    }
}
