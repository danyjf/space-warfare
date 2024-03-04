// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_Planet.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_Planet : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY()
    class UCPP_GravityComponent* MyGravityComponent;

    UFUNCTION(BlueprintCallable)
    void SetRotationAtEpoch(const FDateTime& Epoch);

    ACPP_Planet();    
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
