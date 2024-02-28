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
    float ThrusterStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TorqueStrength;

    UPROPERTY(BlueprintReadWrite)
    UStaticMeshComponent* StaticMeshComponent;

    UFUNCTION(BlueprintCallable)
    void ActivateThruster() { bThrusterIsActive = true; }

    UFUNCTION(BlueprintCallable)
    void DeactivateThruster() { bThrusterIsActive = false; }

	// Sets default values for this component's properties
	UCPP_Thruster();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    bool bThrusterIsActive;
};
