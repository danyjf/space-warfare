// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_Thruster.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_Thruster : public UActorComponent
{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxThrusterStrength;

    /** Consumption of fuel for this thruster in liters per second */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FuelConsumption;

    UFUNCTION(BlueprintCallable)
    void SetThrusterDirectionInECICoordinates(FVector Direction);

    UFUNCTION(BlueprintCallable)
    void SetThrusterDirectionInLocalCoordinates(FVector Direction);

    UFUNCTION(BlueprintCallable)
    void ActivateThruster(float Strength, float Duration = 0.0f);

    UFUNCTION(BlueprintCallable)
    void DeactivateThruster();

	// Sets default values for this component's properties
	UCPP_Thruster();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    bool bThrusterIsActive;
    FVector ThrusterDirection;  // Thruster direction in local space
    float ThrusterDuration;
    float ThrusterTimer;
    float ThrusterStrength;
    class UStaticMeshComponent* StaticMeshComponent;
    class UCPP_FuelTank* FuelTank;
};
