// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineMeshComponent.h"
#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_OrbitSpline.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_OrbitSpline : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* SplineMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SplineMeshScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int NumberOfPoints;

    UFUNCTION(BlueprintCallable)
    void UpdateOrbit(FOrbitalElements OrbitalElements, class ACPP_Planet* Planet);

    virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_OrbitSpline();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class USplineComponent* SplineComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TArray<USplineMeshComponent*> SplineMeshComponents;

    void CreateSplineMeshComponent(const FVector& StartPoint, const FVector& StartTangent, const FVector& EndPoint, const FVector& EndTangent);
};
