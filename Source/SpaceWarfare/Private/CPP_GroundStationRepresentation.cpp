// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationRepresentation.h"
#include "CPP_Planet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ACPP_GroundStationRepresentation::ACPP_GroundStationRepresentation()
{
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    Root->SetIsReplicated(true);

    DetectionConeVisualization = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionConeVisualization"));
    DetectionConeVisualization->SetupAttachment(Root);
    DetectionConeVisualization->SetEnableGravity(false);
    DetectionConeVisualization->SetCollisionProfileName(TEXT("NoCollision"));
    DetectionConeVisualization->SetCastShadow(false);

    DetectionFieldOfView = 90.0f;
    DetectionVisualizationHeight = 700.0f;
}

void ACPP_GroundStationRepresentation::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

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
void ACPP_GroundStationRepresentation::BeginPlay()
{
	Super::BeginPlay();

    UMaterialInterface* Material = DetectionConeVisualization->GetMaterial(0);
    DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
    DetectionConeVisualization->SetMaterial(0, DynamicMaterial);

    if (!HasAuthority())
    {
        SetActorHiddenInGame(false);
    }
    else if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != GetOwner())
    {
        SetActorHiddenInGame(true);
    }
}

void ACPP_GroundStationRepresentation::SetConeColor(FLinearColor Color)
{
    DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
}
