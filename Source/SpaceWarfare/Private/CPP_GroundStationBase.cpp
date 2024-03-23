// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationBase.h"
#include "CPP_Planet.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACPP_GroundStationBase::ACPP_GroundStationBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    DetectionCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionCone"));
    DetectionCone->SetupAttachment(Root);
    DetectionCone->SetEnableGravity(false);
    DetectionCone->SetCollisionProfileName(TEXT("Trigger"));
    DetectionCone->SetCastShadow(false);
    DetectionCone->SetHiddenInGame(true);
    DetectionCone->SetVisibility(false);

    DetectionConeVisualization = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionConeVisualization"));
    DetectionConeVisualization->SetupAttachment(Root);
    DetectionConeVisualization->SetEnableGravity(false);
    DetectionConeVisualization->SetCollisionProfileName(TEXT("NoCollision"));
    DetectionConeVisualization->SetCastShadow(false);

    DetectionFieldOfView = 90.0f;
    DetectionHeight = 50000.0f;
    DetectionVisualizationHeight = 700.0f;
}

void ACPP_GroundStationBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Set the detection cone size
    if (!DetectionCone)
    {
        return;
    }

    float RadiusOfBase = DetectionHeight * tan(UKismetMathLibrary::DegreesToRadians(DetectionFieldOfView / 2.0f));
    DetectionCone->SetWorldScale3D(FVector(RadiusOfBase, RadiusOfBase, DetectionHeight));

    float VisualizationRadiusOfBase = DetectionVisualizationHeight * tan(UKismetMathLibrary::DegreesToRadians(DetectionFieldOfView / 2.0f));
    DetectionConeVisualization->SetWorldScale3D(FVector(VisualizationRadiusOfBase, VisualizationRadiusOfBase, DetectionVisualizationHeight));

    // Set the ground station position on the earth surface
    if (!Planet)
    {
        return;
    }

    // Set the location from the latitude and longitude
    FVector ECILocation = UUniverse::ConvertGeographicCoordinatesToECILocation(Planet, GeographicCoordinates);
    ECILocation.Y = -ECILocation.Y;     // convert coordinates to left handed system
    SetActorLocation(ECILocation);

    // Set the rotation to be orthogonal to earths surface
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Planet->GetActorLocation(), GetActorLocation()));
}

// Called when the game starts or when spawned
void ACPP_GroundStationBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_GroundStationBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStationBase::SetGeographicCoordinates(const FGeographicCoordinates& Value)
{
    GeographicCoordinates = Value;

    // Set the location from the latitude and longitude
    FVector ECILocation = UUniverse::ConvertGeographicCoordinatesToECILocation(Planet, GeographicCoordinates);
    ECILocation.Y = -ECILocation.Y;     // convert coordinates to left handed system
    SetActorLocation(ECILocation);

    // Set the rotation to be orthogonal to earths surface
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Planet->GetActorLocation(), GetActorLocation()));
}
