// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStation.h"
#include "CPP_GroundStationManager.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_Satellite.h"
#include "CPP_Planet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACPP_GroundStation::ACPP_GroundStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    Root->SetIsReplicated(true);

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

void ACPP_GroundStation::OnConstruction(const FTransform& Transform)
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
void ACPP_GroundStation::BeginPlay()
{
	Super::BeginPlay();

    if (!HasAuthority())
    {
        SetActorHiddenInGame(false);

        GroundStationManager = Cast<ACPP_GroundStationManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GroundStationManager::StaticClass()));
        GroundStationManager->GroundStations.Add(this);

        return;
    }

	MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    GroundStationManager = MultiplayerGameMode->GetGroundStationManagers()[OwnerPlayerID];
    GroundStationManager->GroundStations.Add(this);

    DetectionCone->OnComponentBeginOverlap.AddDynamic(this, &ACPP_GroundStation::OnComponentBeginOverlap);
    DetectionCone->OnComponentEndOverlap.AddDynamic(this, &ACPP_GroundStation::OnComponentEndOverlap);

    // Hide enemy ground stations on listen server
    if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != GetOwner())
    {
        SetActorHiddenInGame(true);
    }
}

// Called every frame
void ACPP_GroundStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ACPP_GroundStation, OwnerPlayerID, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(ACPP_GroundStation, Name, COND_InitialOnly);
    DOREPLIFETIME_CONDITION(ACPP_GroundStation, DetectionFieldOfView, COND_InitialOnly);
}

void ACPP_GroundStation::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (!HasAuthority())
    {
        return;
    }

    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(OtherActor);
    if (Satellite)
    {
        GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
    }
}

void ACPP_GroundStation::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority())
    {
        return;
    }

    ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(OtherActor);
    if (Satellite)
    {
        GroundStationManager->SatelliteExitedOverpassArea(Satellite);
    }
}
