// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_SatelliteLauncher.h"
#include "CPP_Satellite.h"
#include "CPP_Planet.h"
#include "CPP_GravityComponent.h"
#include "CPP_GravityManager.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_BotGameMode.h"
#include "CPP_GroundStationManager.h"
#include "CPP_CameraOrbitController.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_SatelliteLauncher::ACPP_SatelliteLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    Root->SetIsReplicated(true);

    OwnerPlayerID = 0;
    LaunchCost = 50;    // Millions
    LaunchDirection = FVector(1.0f, 0.0f, 0.0f);
}

void ACPP_SatelliteLauncher::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        SetActorHiddenInGame(false);
    }
    else if (HasAuthority())
    {
        MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        // Hide enemy satellite launcher on listen server
        if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != GetOwner())
        {
            SetActorHiddenInGame(true);
        }
    }
}

// Called every frame
void ACPP_SatelliteLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_SatelliteLauncher::SetGeographicCoordinates(const FGeographicCoordinates& Value)
{
    GeographicCoordinates = Value;

    // Set the location from the latitude and longitude
    FVector ECILocation = UUniverse::ConvertGeographicCoordinatesToECILocation(Planet, GeographicCoordinates);
    ECILocation.Y = -ECILocation.Y;     // convert coordinates to left handed system
    SetActorLocation(ECILocation);

    // Set the rotation to be orthogonal to earths surface
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Planet->GetActorLocation(), GetActorLocation()));
}

FVector ACPP_SatelliteLauncher::GetLocationFromHeight(float Height)
{
    if (!Planet)
    {
        Planet = Cast<ACPP_Planet>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_Planet::StaticClass()));
    }

    // Add radius of earth to heigth
    Height += Planet->GetActorScale().X / 2;
    return LaunchDirection * Height;
}

FVector ACPP_SatelliteLauncher::GetVelocityFromAngle(float Angle, float Value)
{
    if (LaunchDirection == FVector(0.0f, 0.0f, 1.0f))
    {
        FVector Velocity = FVector(1.0f, 0.0f, 0.0f) * Value;
        Velocity = Velocity.RotateAngleAxis(Angle, LaunchDirection);
        return Velocity;
    }

    FVector PerpendicularToLaunchDirection = UKismetMathLibrary::Cross_VectorVector(LaunchDirection, FVector(0.0f, 0.0f, 1.0f));
    FVector Velocity = PerpendicularToLaunchDirection * Value;
    Velocity = Velocity.RotateAngleAxis(Angle, LaunchDirection);
    return Velocity;
}

void ACPP_SatelliteLauncher::ServerLaunchSatellite_Implementation(FOrbitalElements OrbitalElements, float Size, float Mass, const FString& Label)
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());
    if (CameraOrbitController->Currency < LaunchCost)
    {
        UKismetSystemLibrary::PrintString(GetWorld(), "Not enough money to launch!!!");
        return;
    }

    FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(OrbitalElements, Planet->GravityComponent->GetGravitationalParameter());

    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
    Satellite->SetActorLocation(OrbitalState.Location);
    Satellite->SetActorScale3D(FVector(Size));
    Satellite->OrbitingPlanet = Planet;
    Satellite->Label = Label;
    Satellite->OwnerPlayerID = OwnerPlayerID;
    Satellite->SetOwner(CameraOrbitController);

    Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
    Satellite->GravityComponent->SetMass(Mass);
    Satellite->GravityComponent->SetGravitationalParameter(MultiplayerGameMode->GravityManager->GravitationalConstant * Mass);

    CameraOrbitController->SpendCurrency(LaunchCost);

    // TODO: Change later, this is just to show the satellite on all players when it is launched
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        if (Cast<ACPP_BotGameMode>(MultiplayerGameMode))
        {
            GroundStationManager->SatelliteExitedOverpassArea(Satellite);
        }
    }
}

void ACPP_SatelliteLauncher::ServerLaunchSatelliteWithOrbitalState_Implementation(FOrbitalState OrbitalState, float Size, float Mass, const FString& Label)
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(GetOwner());
    if (CameraOrbitController->Currency < LaunchCost)
    {
        UKismetSystemLibrary::PrintString(GetWorld(), "Not enough money to launch!!!");
        return;
    }

    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(GetWorld()->SpawnActor(SatelliteBlueprintClass));
    Satellite->SetActorLocation(OrbitalState.Location);
    Satellite->SetActorScale3D(FVector(Size));
    Satellite->OrbitingPlanet = Planet;
    Satellite->Label = Label;
    Satellite->OwnerPlayerID = OwnerPlayerID;
    Satellite->SetOwner(CameraOrbitController);

    Satellite->GravityComponent->SetVelocity(OrbitalState.Velocity);
    Satellite->GravityComponent->SetMass(Mass);
    Satellite->GravityComponent->SetGravitationalParameter(MultiplayerGameMode->GravityManager->GravitationalConstant * Mass);

    CameraOrbitController->SpendCurrency(LaunchCost);

    // TODO: Change later, this is just to show the satellite on all players when it is launched
    for (ACPP_GroundStationManager* GroundStationManager : MultiplayerGameMode->GetGroundStationManagers())
    {
        GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        if (Cast<ACPP_BotGameMode>(MultiplayerGameMode))
        {
            GroundStationManager->SatelliteExitedOverpassArea(Satellite);
        }
    }
}
