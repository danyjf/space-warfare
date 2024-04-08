// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationBase.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationBase : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGeographicCoordinates GeographicCoordinates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTextureRenderTarget2D* CostMaterialRenderTarget;

    UFUNCTION(BlueprintCallable)
    void UpdateCost();

    UFUNCTION(BlueprintCallable)
    void SetGeographicCoordinates(const FGeographicCoordinates& Value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStationBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    TMap<FLinearColor, int> CostTable;

    bool AreSimilarColors(FLinearColor ColorA, FLinearColor ColorB, float MaxDiffPercentage);
};
