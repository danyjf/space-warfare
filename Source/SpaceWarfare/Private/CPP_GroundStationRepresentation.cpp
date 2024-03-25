// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStationRepresentation.h"

// Called when the game starts or when spawned
void ACPP_GroundStationRepresentation::BeginPlay()
{
	Super::BeginPlay();

    UMaterialInterface* Material = DetectionConeVisualization->GetMaterial(0);
    DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
    DetectionConeVisualization->SetMaterial(0, DynamicMaterial);
}

void ACPP_GroundStationRepresentation::SetConeColor(FLinearColor Color)
{
    DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
}
