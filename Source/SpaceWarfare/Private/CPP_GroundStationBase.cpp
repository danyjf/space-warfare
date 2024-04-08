// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationBase.h"
#include "CPP_Planet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_GroundStationBase::ACPP_GroundStationBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Cost = 20;
    CostTable.Add(FLinearColor(0.0f, 1.0f, 0.0f), 20);
    CostTable.Add(FLinearColor(0.0f, 0.0f, 1.0f), 40);
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

void ACPP_GroundStationBase::UpdateCost()
{
    // Get UVs from trace hit
    FHitResult Hit;
    FVector2D HitUV;
    FVector TraceStart = GetActorLocation() + GetActorForwardVector() * 10.0f;
    FVector TraceEnd = TraceStart + GetActorForwardVector() * -1000.0f;

    FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;       // Needs to be true to get the UV
    QueryParams.bReturnFaceIndex = true;    // Needs to be true to get the UV

    GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);

    if (!Hit.bBlockingHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ground station line trace didn't hit Earth!"));
        return;
    }
    UGameplayStatics::FindCollisionUV(Hit, 0, HitUV);

    // Get color on UV
    UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), CostMaterialRenderTarget, Planet->GroundStationCostMaterial);
    FLinearColor Color = UKismetRenderingLibrary::ReadRenderTargetUV(GetWorld(), CostMaterialRenderTarget, HitUV.X, HitUV.Y);

    UE_LOG(LogTemp, Log, TEXT("Hit Color: %s"), *Color.ToString());

    for (const TPair<FLinearColor, int>& Elem : CostTable)
    {
        if (AreSimilarColors(Elem.Key, Color, 0.1f))
        {
            Cost = Elem.Value;
            break;
        }
    }
}

bool ACPP_GroundStationBase::AreSimilarColors(FLinearColor ColorA, FLinearColor ColorB, float MaxDiffPercentage)
{
    float DiffR = UKismetMathLibrary::Abs(ColorA.R - ColorB.R);
    float DiffG = UKismetMathLibrary::Abs(ColorA.G - ColorB.G);
    float DiffB = UKismetMathLibrary::Abs(ColorA.B - ColorB.B);

    if (DiffR > MaxDiffPercentage || DiffG > MaxDiffPercentage || DiffB > MaxDiffPercentage)
    {
        return false;
    }

    return true;
}
