// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_CameraOrbitController.generated.h"


// Forward Declarations
class UCPP_CameraOrbitableComponent;
class USpringArmComponent;


UENUM(BlueprintType)
enum class EInputMode : uint8 
{
    GROUNDSTATIONINPUT = 0  UMETA(DisplayName = "GroundStationInput"),
    GODMODEINPUT = 1        UMETA(DisplayName = "GodModeInput")
};


/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_CameraOrbitController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* OrbitingActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APawn* PlayerPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCPP_CameraOrbitableComponent* CameraOrbitableComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInputMode InputMode;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
