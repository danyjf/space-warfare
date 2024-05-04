// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_OrbitSpline.h"
#include "CPP_Planet.h"
#include "CPP_GravityComponent.h"

#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACPP_OrbitSpline::ACPP_OrbitSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    SplineComponent->SetupAttachment(Root);

    SplineMeshScale = 0.25f;
    NumberOfPoints = 24;
    bIsHyperbolic = false;
    bIsVisualizationEnabled = false;
}

void ACPP_OrbitSpline::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (!SplineMesh || !SplineComponent)
    {
        return;
    }

    UMaterialInterface* Material = SplineMesh->GetMaterial(0);
    DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
    
    SplineComponent->ClearSplinePoints();

    float Angle = 0.0f;
    float IncrementAngle = 360.0f / NumberOfPoints;
    float Radius = 300.0f;
    for (int i = 0; i < NumberOfPoints; i++)
    {
        FVector PointPosition;
        PointPosition.X = cos(UKismetMathLibrary::DegreesToRadians(Angle)) * Radius;
        PointPosition.Y = sin(UKismetMathLibrary::DegreesToRadians(Angle)) * Radius;

        SplineComponent->AddSplinePoint(PointPosition, ESplineCoordinateSpace::World);

        Angle += IncrementAngle;
    }

    for (int i = 0; i < (SplineComponent->GetNumberOfSplinePoints() - 1); i++)
    {
        const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
        const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

        CreateSplineMeshComponent(StartPoint, StartTangent, EndPoint, EndTangent);
    }

    if (SplineComponent->IsClosedLoop())
    {

        const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local);
        const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineComponent->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local);
        const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
        const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::Local);

        CreateSplineMeshComponent(StartPoint, StartTangent, EndPoint, EndTangent);
    }
}

// Called when the game starts or when spawned
void ACPP_OrbitSpline::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_OrbitSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_OrbitSpline::CreateSplineMeshComponent(const FVector& StartPoint, const FVector& StartTangent, const FVector& EndPoint, const FVector& EndTangent)
{
    USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);

    SplineMeshComponent->SetStaticMesh(SplineMesh);
    SplineMeshComponent->SetMobility(EComponentMobility::Movable);
    SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
    SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
    SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
    SplineMeshComponent->SetCastShadow(false);
    SplineMeshComponent->SetMaterial(0, DynamicMaterial);

    SplineMeshComponent->SetForwardAxis(ForwardAxis);
    SplineMeshComponent->SetStartScale(FVector2D(SplineMeshScale));
    SplineMeshComponent->SetEndScale(FVector2D(SplineMeshScale));

    SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);

    SplineMeshComponents.Add(SplineMeshComponent);
}

void ACPP_OrbitSpline::UpdateOrbit(FOrbitalElements OrbitalElements, ACPP_Planet* Planet)
{
    // Hide orbit when it is hyperbolic
    //if (OrbitalElements.Eccentricity >= 1.0f || !bIsVisualizationEnabled)
    //{
        //bIsHyperbolic = true;
        //SetActorHiddenInGame(true);
        //return;
    //}

    // Show orbit after is no longer hyperbolic
    if (bIsHyperbolic)
    {
        bIsHyperbolic = false;
        SetActorHiddenInGame(false);
    }

    OrbitalElements.MeanAnomaly = -180.0f;
    float IncrementAngle = 360.0f / NumberOfPoints;

    for (int i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
    {
        FOrbitalState OrbitalState = UUniverse::ConvertOrbitalElementsToOrbitalState(OrbitalElements, Planet->GetComponentByClass<UCPP_GravityComponent>()->GetGravitationalParameter());

        SplineComponent->SetWorldLocationAtSplinePoint(i, OrbitalState.Location);

        OrbitalElements.MeanAnomaly += IncrementAngle;
    }
    
    for (int i = 0; i < SplineMeshComponents.Num() - 1; i++)
    {
        const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
        const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

        SplineMeshComponents[i]->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
    }

    const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineMeshComponents.Num() - 1, ESplineCoordinateSpace::Local);
    const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineMeshComponents.Num() - 1, ESplineCoordinateSpace::Local);
    const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
    const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::Local);
    SplineMeshComponents[SplineMeshComponents.Num() - 1]->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
}

void ACPP_OrbitSpline::SetColor(FLinearColor Color)
{
    DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
}
