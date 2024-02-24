// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GroundStation.h"
#include "CPP_Planet.h"
#include "CPP_Satellite.h"
#include "CPP_GroundStationManager.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACPP_GroundStation::ACPP_GroundStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    DetectionCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionCone"));
    DetectionCone->SetupAttachment(Root);
    DetectionCone->SetEnableGravity(false);
    DetectionCone->SetCollisionProfileName(TEXT("Trigger"));
    DetectionCone->SetCastShadow(false);

    DetectionFieldOfView = 90.0f;
    DetectionHeight = 2000.0f;
}

void ACPP_GroundStation::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // set the detection cone size
    if (!DetectionCone)
    {
        return;
    }

    float RadiusOfBase = DetectionHeight * tan(UKismetMathLibrary::DegreesToRadians(DetectionFieldOfView / 2.0f));
    DetectionCone->SetWorldScale3D(FVector(RadiusOfBase, RadiusOfBase, DetectionHeight));

    // set the ground station position on the earth surface
    if (!Planet)
    {
        return;
    }

    // set the location from the latitude and longitude
    FVector ECILocation = UUniverse::ConvertGeographicCoordinatesToECILocation(Planet, GeographicCoordinates);
    ECILocation.Y = -ECILocation.Y;     // convert coordinates to left handed system
    SetActorLocation(ECILocation);

    // set the rotation to be orthogonal to earths surface
    SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Planet->GetActorLocation(), GetActorLocation()));
}

// Called when the game starts or when spawned
void ACPP_GroundStation::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_GroundStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //for (ACPP_Satellite* Satellite : OverpassingSatellites)
    //{
    //    // Update the data of the tracked satellites while their overpassing
    //    TrackedSatellites.Emplace(Satellite, Satellite->GetSatelliteStatus());
    //    UE_LOG(LogTemp, Warning, TEXT("%s: Position(%s), Rotation(%s), Velocity(%s)"), 
    //        *Satellite->Name, 
    //        *TrackedSatellites.Find(Satellite)->Position.ToString(),
    //        *TrackedSatellites.Find(Satellite)->Rotation.ToString(),
    //        *TrackedSatellites.Find(Satellite)->Velocity.ToString()
    //    )
    //}
}

void ACPP_GroundStation::AddOverpassingSatellite(ACPP_Satellite* Satellite)
{
    OverpassingSatellites.Add(Satellite);
}

void ACPP_GroundStation::RemoveOverpassingSatellite(ACPP_Satellite* Satellite)
{
    OverpassingSatellites.Remove(Satellite);
}
